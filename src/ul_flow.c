#include "../include/ul_flow.h"
#include "../include/logger.h"
#include "../include/location.h"
#include "../include/ul_assert.h"
#include "../include/lexer.h"
#include "../include/ul_allocator.h"
#include "../include/ul_compiler_globals.h"
#include "../include/ul_dyn_arrays.h"
#include <stdlib.h>

void ul_start(int argc, char **argv) {
  (void)argc;
  (void)argv;
  create_logger(&ul_global_logger);
  ul_logger_info("Started Unilang compiler");
  unsigned int arena = new_arena(1024);
  set_arena(arena);
  lexer_t l;
  new_lexer(&l, "examples/hello_world.ul");
  putchar(10);
  while (!is_end_of_file(l)) {
    char c = consume_char(&l);
    putchar(c);
  }
  putchar(10);
  putchar(10);

  token_t tok = {0};
  tok.kind = T_PLUS;
  token_array_t toks = new_tok_dyn();
  for (size_t i = 0; i < 1024; i++) {
    ul_dyn_append(&toks, tok);
    token_t res = dyn_tok_get(toks, i);
    ul_assert(res.kind == T_PLUS, "New token is different from old one");
    char buffer[128] = {0};
    sprintf(buffer, "Success on iteration i = %ld", i);
    ul_logger_info(buffer);
  }

  ul_dyn_destroy(toks);

  destroy_arena(arena);
}

void ul_exit(unsigned char exit_code) {
  clear_allocator();
  ul_destroy_logger();
  exit(exit_code);
}
void ul_end(void) {
  ul_logger_info("Ended Unilang compiler");
  ul_exit(0);
}