#include "../include/ul_flow.h"
#include "../include/logger.h"
#include "../include/location.h"
#include "../include/ul_assert.h"
#include "../include/lexer.h"
#include "../include/ul_allocator.h"

#include <stdlib.h>

logger_t logger;

void ul_start(int argc, char **argv) {
  (void)argc;
  (void)argv;
  create_logger(&logger);
  ul_set_logger(&logger);
  logger_info(logger, "Started Unilang compiler");
  new_arena(1024, false);
  lexer_t l;
  new_lexer(&l, "examples/hello_world.ul");
  while (!is_end_of_file(l)) {
    char c = consume_char(&l);
    printf("%c", c);
  }
  printf("\n");
  destroy_arena();
}

void ul_exit(unsigned char exit_code) {
  destroy_logger(logger);
  exit(exit_code);
}
void ul_end(void) {
  logger_info(logger, "Ended Unilang compiler");
  ul_exit(0);
}