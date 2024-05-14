#include "../include/ul_flow.h"
#include "../include/lexer.h"
#include "../include/logger.h"
#include "../include/parser.h"
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
  while (step_lexer(&l))
    ;

  ul_logger_info("\n\n************************************\n          "
                 "TOKENS\n************************************\n");

  size_t toks_length = ul_dyn_length(l.toks);
  for (size_t i = 0; i < toks_length; i++) {
    token_t tok = dyn_tok_get(l.toks, i);
    print_token(tok);
  }

  parser_t p = new_parser(l.toks);

  while (!is_parser_done(p)) {
    token_t tok = consume_parser(&p);
    ul_logger_info_location(tok.location, token_kind_to_str(tok.kind));
  }

  destroy_lexer(l);
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