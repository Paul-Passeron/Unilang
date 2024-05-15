#include "../include/ul_flow.h"
#include "../include/generator.h"
#include "../include/lexer.h"
#include "../include/logger.h"
#include "../include/parser.h"
#include "../include/ul_allocator.h"
#include "../include/ul_compiler_globals.h"
#include <stdlib.h>

void ul_start(int argc, char **argv) {
  (void)argc;
  (void)argv;
  create_logger(&ul_global_logger);
  ul_logger_info("Started Unilang compiler");
  unsigned int arena = new_arena(1024);
  set_arena(arena);

  ul_logger_info("Starting Lexer");
  lexer_t l;
  new_lexer(&l, "examples/hello_world.ul");
  lex_program(&l);

  ul_logger_info("Starting Parser");
  parser_t p = new_parser(l.toks);
  ast_t prog = parse_program(&p);
  ul_logger_info("File successfully parsed");

  ul_logger_info("Starting Generator");
  set_generator_target("a.out.c");
  generate_program(prog);

  destroy_lexer(l);
  destroy_parser(p);
  destroy_generator();
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