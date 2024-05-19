// PARSER SOURCE FILE
// Paul Passeron

#include "../include/parser.h"
#include "../include/ul_allocator.h"
#include "../include/ul_assert.h"
#include "../include/ul_ast.h"
#include "../include/ul_compiler_globals.h"
#include "../include/ul_dyn_arrays.h"

unsigned int parser_arena;

parser_t new_parser(token_array_t toks) {
  parser_t res;
  size_t l = ul_dyn_length(toks);
  size_t to_allocate = 4 * l * (sizeof(ast_t) + sizeof(struct ast_struct_t));
  unsigned int old_arena = get_arena();
  unsigned int arena = new_arena(to_allocate);
  parser_arena = arena;
  set_arena(old_arena);
  res.arena = arena;
  res.toks = toks;
  res.current_index = 0;
  return res;
}

void destroy_parser(parser_t p) { destroy_arena(p.arena); }

token_t peek_parser(parser_t p) {
  if (is_parser_done(p))
    return (token_t){0};
  return dyn_tok_get(p.toks, p.current_index);
}

bool is_parser_done(parser_t p) {
  return p.current_index >= ul_dyn_length(p.toks);
}

void expect(parser_t p, token_kind_t kind) {
  ul_assert(!is_parser_done(p),
            "Could not expect as the end of the tokens was reached");
  token_t tok = peek_parser(p);
  if (tok.kind != kind) {

    char buffer[128] = {0};
    sprintf(buffer, "Expected kind %s but got: %s!", token_kind_to_str(kind),
            token_kind_to_str(tok.kind));
    ul_assert_location(tok.location, false, buffer);
  }
}

void expect_lexeme(parser_t p, const char *lexeme) {
  ul_assert(!is_parser_done(p),
            "Could not expect lexeme as the end of the tokens was reached");
  token_t tok = peek_parser(p);
  if (!streq(tok.lexeme, lexeme)) {
    char buffer[128] = {0};
    sprintf(buffer, "Expected lexeme %s but got: %s!", lexeme, tok.lexeme);
    ul_assert_location(tok.location, false, buffer);
  }
}

token_t consume_parser(parser_t *p) {
  if (is_parser_done(*p))
    return (token_t){0};
  token_t res = peek_parser(*p);
  p->current_index++;
  return res;
}

bool matches_lexeme(parser_t p, const char *lexeme) {
  if (is_parser_done(p))
    return false;
  token_t tok = peek_parser(p);
  return streq(tok.lexeme, lexeme);
}

// TODO: handle including files
ast_t parse_program(parser_t *p) {
  ast_t prog = new_prog();
  while (!is_parser_done(*p)) {
    ast_t stmt = parse_statement(p);
    ul_dyn_append(&prog->as.prog->prog, stmt);
  }
  return prog;
}

token_kind_t peek_kind(parser_t p) {
  if (is_parser_done(p))
    return -1;
  return peek_parser(p).kind;
}

bool is_fundef(parser_t p) {
  // pattern: let <funname>(...
  if (peek_kind(p) != T_WORD) {
    return false;
  }
  token_t tok = consume_parser(&p);
  if (!streq(tok.lexeme, "let")) {
    return false;
  }
  if (peek_kind(p) != T_WORD) {
    return false;
  }
  consume_parser(&p);
  if (peek_kind(p) != T_OPENPAREN) {
    return false;
  }
  return true;
}

bool is_funcall(parser_t p) {
  // pattern: <funname>(...
  if (peek_kind(p) != T_WORD) {
    return false;
  }
  consume_parser(&p);
  if (peek_kind(p) != T_OPENPAREN) {
    return false;
  }
  return true;
}

ast_t parse_leaf(parser_t *p) {
  ast_t res = NULL;
  token_t tok = peek_parser(*p);
  if (tok.kind == T_OPENPAREN) {
    ul_logger_info_location(tok.location, "parsing leaf as paren expression");
    consume_parser(p);
    res = parse_expression(p);
    expect(*p, T_CLOSEPAREN);
    consume_parser(p);
  } else if (is_funcall(*p)) {
    ul_logger_info_location(tok.location, "parsing leaf as funcall");
    res = parse_funcall(p);
  } else if (tok.kind == T_WORD) {
    ul_logger_info_location(tok.location, "parsing leaf as identifier");
    res = parse_identifier(p);
  } else if (tok.kind == T_STRLIT) {
    ul_logger_info_location(tok.location, "parsing leaf as strlit");
    res = parse_strlit(p);
  } else if (tok.kind == T_NUMLIT) {
    ul_logger_info_location(tok.location, "parsing leaf as numlit");
    res = parse_numlit(p);
  } else if (tok.kind == T_CHARLIT) {
    ul_logger_info_location(tok.location, "parsing leaf as charlit");
    res = parse_charlit(p);
  }
  ul_assert_location(tok.location, res != NULL, "Could not parse leaf");
  tok = peek_parser(*p);
  if (tok.kind != T_OPENBRACKET)
    return res;
  ul_logger_info_location(tok.location, "Adding indexing to leaf");

  consume_parser(p);
  ast_t expr = parse_expression(p);
  expect(*p, T_CLOSEBRACKET);
  consume_parser(p);
  return new_index(res, expr);
}

bool is_kind_op(token_kind_t t) { return get_precedence(t) != -1; }

ast_t parse_expression_aux(parser_t *p, int min_prec) {
  ast_t left = parse_leaf(p);
  while (true) {
    token_kind_t lookahead = peek_kind(*p);
    if (!is_kind_op(lookahead))
      break;
    int op_prec = get_precedence(lookahead);
    if (op_prec < min_prec)
      break;
    consume_parser(p);
    ast_t right = parse_expression_aux(p, op_prec);
    left = new_binop(lookahead, left, right);
  }
  return left;
}

ast_t parse_identifier(parser_t *p) {
  expect(*p, T_WORD);
  return new_iden(consume_parser(p).lexeme);
}

ast_t parse_expression(parser_t *p) { return parse_expression_aux(p, -1); }

ast_t parse_strlit(parser_t *p) {
  expect(*p, T_STRLIT);
  token_t tok = consume_parser(p);

  return new_strlit(tok.lexeme);
}
ast_t parse_numlit(parser_t *p) {
  expect(*p, T_NUMLIT);
  token_t tok = consume_parser(p);
  char *tmp = tok.lexeme;
  bool has_point = false;
  while (*tmp) {
    if (*(tmp++) == '.') {
      has_point = true;
      break;
    }
  }
  return new_numlit(tok.lexeme, has_point);
}
ast_t parse_charlit(parser_t *p) {
  expect(*p, T_CHARLIT);
  token_t tok = consume_parser(p);
  return new_charlit(tok.lexeme);
}

ast_t parse_fundef_param(parser_t *p) {
  expect(*p, T_WORD);
  token_t tok = consume_parser(p);
  expect(*p, T_COLON);
  consume_parser(p);
  ast_t type = parse_identifier(p);
  return new_fundef_param(type, tok.lexeme);
}

ast_t parse_fundef(parser_t *p) {
  // let <name>([params])

  expect_lexeme(*p, "let");
  consume_parser(p);

  token_t tok = consume_parser(p);
  ast_array_t params = new_ast_dyn();
  ast_array_t body = new_ast_dyn();
  expect(*p, T_OPENPAREN);
  consume_parser(p);

  while (peek_kind(*p) != T_CLOSEPAREN) {
    ast_t param = parse_fundef_param(p);
    ul_dyn_append(&params, param);
    if (peek_kind(*p) == T_CLOSEPAREN)
      break;
    expect(*p, T_COMMA);
    consume_parser(p);
  }

  expect(*p, T_CLOSEPAREN);
  consume_parser(p);

  expect(*p, T_COLON);
  consume_parser(p);

  ast_t ret_type = parse_identifier(p);

  expect(*p, T_BIGARR);
  consume_parser(p);

  expect(*p, T_OPENBRACE);
  consume_parser(p);

  while (peek_kind(*p) != T_CLOSEBRACE) {
    ast_t stmt = parse_statement(p);
    ul_dyn_append(&body, stmt);
  }

  expect(*p, T_CLOSEBRACE);
  consume_parser(p);

  return new_fundef(params, ret_type, tok.lexeme, body);
}

ast_t parse_funcall(parser_t *p) {
  expect(*p, T_WORD);
  token_t tok = consume_parser(p);

  expect(*p, T_OPENPAREN);
  consume_parser(p);

  ast_array_t args = new_ast_dyn();

  while (peek_kind(*p) != T_CLOSEPAREN) {
    ast_t expr = parse_expression(p);
    ul_dyn_append(&args, expr);
    if (peek_kind(*p) == T_CLOSEPAREN)
      break;
    expect(*p, T_COMMA);
    consume_parser(p);
  }

  expect(*p, T_CLOSEPAREN);
  consume_parser(p);

  return new_funcall(tok.lexeme, args);
}

ast_t parse_statement(parser_t *p) {
  if (is_parser_done(*p))
    return NULL;

  token_t tok = peek_parser(*p);

  if (tok.kind == T_OPENBRACE) {
    return parse_compound(p);
  }
  if (streq(tok.lexeme, "if")) {
    ul_logger_info_location(peek_parser(*p).location,
                            "Parsing current statement as if");
    return parse_ifstmt(p);
  }
  if (streq(tok.lexeme, "return")) {
    ul_logger_info_location(peek_parser(*p).location,
                            "Parsing current statement as return");
    return parse_returnstmt(p);
  }
  if (streq(tok.lexeme, "loop")) {
    ul_logger_info_location(peek_parser(*p).location,
                            "Parsing current statement as loop");
    return parse_loop(p);
  }
  if (streq(tok.lexeme, "while")) {
    ul_logger_info_location(peek_parser(*p).location,
                            "Parsing current statement as while");
    return parse_whilestmt(p);
  }
  if (is_fundef(*p)) {
    ul_logger_info_location(peek_parser(*p).location,
                            "Parsing current statement as fundef");
    return parse_fundef(p);
  }
  if (streq(tok.lexeme, "let")) {

    ul_logger_info_location(peek_parser(*p).location,
                            "Parsing current statement as vardef");
    return parse_vardef(p);
  }

  ul_logger_info_location(peek_parser(*p).location,
                          "Parsing current statement as expression statement");
  ast_t expr = parse_expression(p);

  expect(*p, T_SEMICOLON);
  consume_parser(p);

  return expr;
}

ast_t parse_compound(parser_t *p) {
  ast_array_t stmts = new_ast_dyn();
  expect(*p, T_OPENBRACE);
  consume_parser(p);
  while (peek_kind(*p) != T_CLOSEBRACE) {
    ast_t stmt = parse_statement(p);
    ul_dyn_append(&stmts, stmt);
  }
  expect(*p, T_CLOSEBRACE);
  consume_parser(p);
  return new_compound(stmts);
}

ast_t parse_ifstmt(parser_t *p) {
  expect_lexeme(*p, "if");
  consume_parser(p);

  ast_t cond = parse_expression(p);

  expect(*p, T_BIGARR);
  consume_parser(p);

  ast_t body = parse_statement(p);

  ast_t else_body = NULL;

  if (streq(peek_parser(*p).lexeme, "else")) {
    consume_parser(p);
    else_body = parse_statement(p);
  }
  return new_if(cond, body, else_body);
}

ast_t parse_returnstmt(parser_t *p) {
  expect_lexeme(*p, "return");
  consume_parser(p);
  ast_t expr = parse_expression(p);
  expect(*p, T_SEMICOLON);
  consume_parser(p);
  return new_return(expr);
}
ast_t parse_whilestmt(parser_t *p) {
  (void)p;
  ul_assert(false, "parse_whilestmt is not implemented yet !");
  return NULL;
}
ast_t parse_loop(parser_t *p) {
  expect_lexeme(*p, "loop");
  consume_parser(p);
  expect(*p, T_WORD);
  char *varname = consume_parser(p).lexeme;
  expect(*p, T_COLON);
  consume_parser(p);
  ast_t init = parse_expression(p);
  bool is_strict;
  if (streq(peek_parser(*p).lexeme, "->")) {
    expect(*p, T_SMALLARR);
    consume_parser(p);
    is_strict = true;
  } else {
    expect(*p, T_SMALLARRLARGE);
    consume_parser(p);
    is_strict = false;
  }
  ast_t end = parse_expression(p);
  expect(*p, T_BIGARR);
  consume_parser(p);
  ast_t body = parse_statement(p);
  return new_loop(varname, init, end, body, is_strict);
}

ast_t parse_vardef(parser_t *p) {
  expect_lexeme(*p, "let");
  consume_parser(p);

  expect(*p, T_WORD);
  token_t tok = consume_parser(p);

  char *name = tok.lexeme;

  expect(*p, T_COLON);
  consume_parser(p);

  ast_t type = parse_identifier(p);

  expect(*p, T_BIGARR);
  consume_parser(p);

  ast_t value = parse_expression(p);

  expect(*p, T_SEMICOLON);
  consume_parser(p);

  return new_vardef(name, type, value);
}
