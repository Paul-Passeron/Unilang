#include "../include/ul_flow.h"
#include "../include/logger.h"
#include "../include/location.h"
#include "../include/ul_assert.h"
#include "../include/lexer.h"
#include "../include/ul_allocator.h"
#include "../include/ul_compiler_globals.h"
#include <stdlib.h>

void ul_start(int argc, char **argv) {
  (void)argc;
  (void)argv;
  create_logger(&ul_global_logger);
  ul_set_logger(&ul_global_logger);

  ul_logger_info("Started Unilang compiler");
  unsigned int arena = new_arena(1024, false);
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
  destroy_arena(arena);
}

void ul_exit(unsigned char exit_code) {
  ul_destroy_logger();
  exit(exit_code);
}
void ul_end(void) {
  ul_logger_info("Ended Unilang compiler");
  ul_exit(0);
}