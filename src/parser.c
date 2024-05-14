// PARSER SOURCE FILE
// Paul Passeron

#include "../include/parser.h"
#include "../include/ul_allocator.h"
#include "../include/ul_assert.h"
#include "../include/ul_ast.h"
#include "../include/ul_compiler_globals.h"
#include "../include/ul_dyn_arrays.h"
#include "../include/ul_flow.h"

unsigned int parser_arena;

parser_t new_parser(token_array_t toks) {
  parser_t res;
  size_t l = ul_dyn_length(toks);
  size_t to_allocate = l * 4 * (sizeof(ast_t) + sizeof(struct ast_struct_t));
  unsigned int old_arena = get_arena();
  unsigned int arena = new_arena(to_allocate);
  parser_arena = arena;
  set_arena(old_arena);
  res.arena = arena;
  res.toks = toks;
  res.current_index = 0;
  return res;
}

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

ast_t parse_identifier(parser_t *p) {
  expect(*p, T_WORD);
  return new_iden(consume_parser(p).lexeme);
}

ast_t parse_expression(parser_t *p) {
  // WARNING: very temporary
  if (peek_kind(*p) == T_STRLIT)
    return parse_strlit(p);
  ul_logger_erro_location(peek_parser(*p).location,
                          "Could not parse expression");
  ul_exit(1);
  return NULL;
}
ast_t parse_strlit(parser_t *p) {
  expect(*p, T_STRLIT);
  token_t tok = consume_parser(p);

  return new_strlit(tok.lexeme);
}
ast_t parse_numlit(parser_t *p) {
  (void)p;
  ul_assert(false, "parse_numlit not implemented yet !");
  return NULL;
}
ast_t parse_charlit(parser_t *p) {
  (void)p;
  ul_assert(false, "parse_charlit not implemented yet !");
  return NULL;
}

ast_t parse_fundef_param(parser_t *p) {
  (void)p;
  ul_assert(false, "parse_fundef_param not implemented yet !");
  return NULL;
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
    if (peek_kind(*p) == T_CLOSEPAREN)
      break;
    expect(*p, T_COMMA);
    consume_parser(p);
    ul_dyn_append(&params, param);
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
    expect(*p, T_COLON);
    consume_parser(p);
  }

  expect(*p, T_CLOSEPAREN);
  consume_parser(p);

  return new_funcall(tok.lexeme, args);
}

ast_t parse_statement(parser_t *p) {
  if (is_parser_done(*p))
    return NULL;
  if (is_fundef(*p)) {
    ul_logger_info_location(peek_parser(*p).location,
                            "Parsing current statement as fundef");
    return parse_fundef(p);
  }
  if (is_funcall(*p)) {
    ul_logger_info_location(peek_parser(*p).location,
                            "Parsing current statement as funcall");
    ast_t res = parse_funcall(p);
    expect(*p, T_SEMICOLON);
    consume_parser(p);
    return res;
  }
  ul_logger_erro_location(peek_parser(*p).location,
                          "Could not parse as statement");
  ul_exit(1);
  return NULL;
}