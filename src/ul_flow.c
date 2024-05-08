#include "../include/ul_flow.h"
#include "../include/logger.h"
#include "../include/location.h"
#include "../include/ul_assert.h"

#define USE_LIBC_MALLOC
#define UL_ALLOC_IMPLEMENTATION
#include "../include/ul_allocator.h"

#include <stdlib.h>

logger_t logger;

void ul_start(int argc, char **argv) {
  (void)argc;
  (void)argv;
  create_logger(&logger);
  ul_set_logger(&logger);
  logger_info(logger, "Started Unilang compiler");
  // char *buffer = alloc_preset(10, 1, 0);
  // sprintf(buffer, "lolilol");
  // logger_info(logger, buffer);
  // free_ul(buffer);
}

void ul_exit(unsigned char exit_code) {
  destroy_logger(logger);
  exit(exit_code);
}
void ul_end(void) {
  logger_info(logger, "Ended Unilang compiler");
  ul_exit(0);
}