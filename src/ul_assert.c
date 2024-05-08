#include "../include/ul_assert.h"
#include "../include/logger.h"
#include "../include/ul_flow.h"
#include <stdlib.h>

logger_t *ul_assert_logger = NULL;

void ul_set_logger(logger_t *logger) { ul_assert_logger = logger; }

void ul_assert(bool value, const char *msg) {
  if (!value) {
    if (ul_assert_logger != NULL) {
      char buff[1024];
      sprintf(buff, "[ASSERTION] %s", msg);
      logger_erro(*ul_assert_logger, buff);
    }
    ul_exit(1);
  }
}