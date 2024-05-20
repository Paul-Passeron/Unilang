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

location_t peek_loc(parser_t p) { return peek_parser(p).location; }

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

bool is_assignement(parser_t p) {
  (void)parse_expression(&p);
  return peek_parser(p).kind == T_BIGARR;
}

// TODO: handle including files
ast_t parse_program(parser_t *p) {
  ast_t prog = new_prog(peek_loc(*p));
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
  location_t loc = peek_loc(*p);
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
  while (peek_kind(*p) == T_DOT) {
    location_t loc = peek_loc(*p);
    consume_parser(p);
    ast_t f;
    if (is_funcall(*p)) {
      f = parse_funcall(p);
    } else {
      f = parse_identifier(p);
    }

    res = new_access(loc, res, f);
  }
  tok = peek_parser(*p);
  if (tok.kind != T_OPENBRACKET)
    return res;
  ul_logger_info_location(tok.location, "Adding indexing to leaf");

  consume_parser(p);
  ast_t expr = parse_expression(p);
  expect(*p, T_CLOSEBRACKET);
  consume_parser(p);
  res = new_index(loc, res, expr);
  while (peek_kind(*p) == T_DOT) {
    location_t loc = peek_loc(*p);
    consume_parser(p);
    ast_t f;
    if (is_funcall(*p)) {
      f = parse_funcall(p);
    } else {
      f = parse_identifier(p);
    }

    res = new_access(loc, res, f);
  }
  return res;
}

bool is_kind_op(token_kind_t t) { return get_precedence(t) != -1; }

ast_t parse_expression_aux(parser_t *p, int min_prec) {
  ast_t left = parse_leaf(p);
  while (true) {
    token_kind_t lookahead = peek_kind(*p);
    location_t loc = peek_loc(*p);

    if (!is_kind_op(lookahead))
      break;
    int op_prec = get_precedence(lookahead);
    if (op_prec < min_prec)
      break;
    consume_parser(p);
    ast_t right = parse_expression_aux(p, op_prec);
    left = new_binop(loc, lookahead, left, right);
  }
  return left;
}

ast_t parse_identifier(parser_t *p) {
  expect(*p, T_WORD);
  location_t loc = peek_loc(*p);
  return new_iden(loc, consume_parser(p).lexeme);
}

ast_t parse_expression(parser_t *p) { return parse_expression_aux(p, -1); }

ast_t parse_strlit(parser_t *p) {
  expect(*p, T_STRLIT);
  location_t loc = peek_loc(*p);
  token_t tok = consume_parser(p);

  return new_strlit(loc, tok.lexeme);
}

ast_t parse_numlit(parser_t *p) {
  expect(*p, T_NUMLIT);
  location_t loc = peek_loc(*p);
  token_t tok = consume_parser(p);
  char *tmp = tok.lexeme;
  bool has_point = false;
  while (*tmp) {
    if (*(tmp++) == '.') {
      has_point = true;
      break;
    }
  }
  return new_numlit(loc, tok.lexeme, has_point);
}
ast_t parse_charlit(parser_t *p) {
  expect(*p, T_CHARLIT);
  location_t loc = peek_loc(*p);
  token_t tok = consume_parser(p);
  return new_charlit(loc, tok.lexeme);
}

ast_t parse_fundef_param(parser_t *p) {
  expect(*p, T_WORD);
  location_t loc = peek_loc(*p);
  token_t tok = consume_parser(p);
  expect(*p, T_COLON);
  consume_parser(p);
  ast_t type = parse_identifier(p);
  return new_fundef_param(loc, type, tok.lexeme);
}

ast_t parse_fundef(parser_t *p) {
  // let <name>([params])

  expect_lexeme(*p, "let");
  location_t loc = peek_loc(*p);
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
  // printf("RET TYPE IS %p\n", ret_type);

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

  return new_fundef(loc, params, ret_type, tok.lexeme, body);
}

ast_t parse_funcall(parser_t *p) {
  expect(*p, T_WORD);
  location_t loc = peek_loc(*p);
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

  return new_funcall(loc, tok.lexeme, args);
}

ast_t parse_tdef_struct(parser_t *p) {
  expect_lexeme(*p, "struct");
  location_t loc = peek_loc(*p);
  consume_parser(p);
  expect(*p, T_WORD);
  char *type_name = consume_parser(p).lexeme;
  expect(*p, T_BIGARR);
  consume_parser(p);
  expect(*p, T_OPENBRACE);
  consume_parser(p);
  str_array_t fields = new_str_dyn();
  str_array_t types = new_str_dyn();
  ast_array_t methods = new_ast_dyn();
  while (peek_kind(*p) != T_CLOSEBRACE) {
    if (is_fundef(*p)) {
      // ul_assert(false, "Method definition is not implemetned yet !");
      ast_t fdef = parse_fundef(p);
      unsigned int old_arena = get_arena();
      char prefix[256] = "__internal_";
      strcat(prefix, type_name);
      strcat(prefix, "_");
      unsigned int arena =
          new_arena(strlen(fdef->as.fundef->name) + sizeof(prefix));
      set_arena(arena);
      char *mangled_name =
          alloc_zero(strlen(fdef->as.fundef->name) + sizeof(prefix), 1);
      strcpy(mangled_name, prefix);
      strcat(mangled_name, fdef->as.fundef->name);
      set_arena(old_arena);
      fdef->as.fundef->name = mangled_name;
      ul_dyn_append(&methods, fdef);
    } else {
      expect(*p, T_WORD);
      ul_dyn_append(&fields, consume_parser(p).lexeme);
      expect(*p, T_COLON);
      consume_parser(p);
      expect(*p, T_WORD);
      ul_dyn_append(&types, consume_parser(p).lexeme);
    }
    if (peek_kind(*p) != T_COMMA) {
      break;
    }
    consume_parser(p);
  }
  expect(*p, T_CLOSEBRACE);
  consume_parser(p);
  // TODO: actually calculates the size of type
  type_t res = {{0}, TY_STRUCT, types, fields, methods, false, false, 0};
  strcpy(res.name, type_name);
  return new_tdef(loc, res);
}

ast_t parse_tdef_enum(parser_t *p) {
  (void)p;
  ul_assert(false, "parse_tdef_enum is not implemented yet !");
  return NULL;
}

ast_t parse_tdef_alias(parser_t *p) {
  (void)p;
  ul_assert(false, "parse_tdef_alias is not implemented yet !");
  return NULL;
}

ast_t parse_tdef(parser_t *p) {
  token_t tok = peek_parser(*p);
  if (streq(tok.lexeme, "struct")) {
    return parse_tdef_struct(p);
  }
  if (streq(tok.lexeme, "enum")) {
    return parse_tdef_enum(p);
  }
  if (streq(tok.lexeme, "alias")) {
    return parse_tdef_alias(p);
  }
  expect_lexeme(*p, "struct");
  return NULL;
}

ast_t parse_assignement(parser_t *p) {
  location_t loc = peek_loc(*p);
  ast_t expr = parse_expression(p);
  expect(*p, T_BIGARR);
  consume_parser(p);
  ast_t value = parse_expression(p);
  expect(*p, T_SEMICOLON);
  consume_parser(p);
  return new_assignement(loc, expr, value);
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
  if (streq(tok.lexeme, "struct")) {
    return parse_tdef(p);
  }
  if (streq(tok.lexeme, "let")) {

    ul_logger_info_location(peek_parser(*p).location,
                            "Parsing current statement as vardef");
    return parse_vardef(p);
  }
  if (is_assignement(*p)) {
    ul_logger_info_location(peek_parser(*p).location,
                            "Parsing current statement as assignement");
    return parse_assignement(p);
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
  location_t loc = peek_loc(*p);
  consume_parser(p);
  while (peek_kind(*p) != T_CLOSEBRACE) {
    ast_t stmt = parse_statement(p);
    ul_dyn_append(&stmts, stmt);
  }
  expect(*p, T_CLOSEBRACE);
  consume_parser(p);
  return new_compound(loc, stmts);
}

ast_t parse_ifstmt(parser_t *p) {
  expect_lexeme(*p, "if");
  location_t loc = peek_loc(*p);

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
  return new_if(loc, cond, body, else_body);
}

ast_t parse_returnstmt(parser_t *p) {
  expect_lexeme(*p, "return");
  location_t loc = peek_loc(*p);

  consume_parser(p);
  ast_t expr = parse_expression(p);
  expect(*p, T_SEMICOLON);
  consume_parser(p);
  return new_return(loc, expr);
}
ast_t parse_whilestmt(parser_t *p) {
  location_t loc = peek_loc(*p);
  expect_lexeme(*p, "while");
  consume_parser(p);
  ast_t expr = parse_expression(p);
  expect(*p, T_BIGARR);
  consume_parser(p);
  ast_t body = parse_statement(p);
  return new_while(loc, expr, body);
}

ast_t parse_loop(parser_t *p) {
  expect_lexeme(*p, "loop");
  location_t loc = peek_loc(*p);

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
  return new_loop(loc, varname, init, end, body, is_strict);
}

ast_t parse_vardef(parser_t *p) {
  expect_lexeme(*p, "let");
  location_t loc = peek_loc(*p);

  consume_parser(p);

  expect(*p, T_WORD);
  token_t tok = consume_parser(p);

  char *name = tok.lexeme;

  expect(*p, T_COLON);
  consume_parser(p);

  ast_t type = parse_identifier(p);

  if (peek_kind(*p) == T_SEMICOLON) {
    consume_parser(p);
    return new_vardef(loc, name, type, NULL);
  } else {
    expect(*p, T_BIGARR);
    consume_parser(p);

    ast_t value = parse_expression(p);

    expect(*p, T_SEMICOLON);
    consume_parser(p);

    return new_vardef(loc, name, type, value);
  }
}
