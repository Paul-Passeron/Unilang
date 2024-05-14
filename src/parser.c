// PARSER SOURCE FILE
// Paul Passeron

#include "../include/parser.h"
#include "../include/ul_allocator.h"
#include "../include/ul_assert.h"
#include "../include/ul_ast.h"
#include "../include/ul_compiler_globals.h"
#include "../include/ul_dyn_arrays.h"

parser_t new_parser(token_array_t toks) {
  parser_t res;
  size_t l = ul_dyn_length(toks);
  size_t to_allocate = l * 4 * (sizeof(ast_t) + sizeof(struct ast_struct_t));
  unsigned int old_arena = get_arena();
  unsigned int arena = new_arena(to_allocate);
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