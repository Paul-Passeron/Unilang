#include "../include/logger.h"
#include "../include/ul_assert.h"
#include "../include/ul_flow.h"
#include <stdlib.h>

static char buff[1024];

void ul_assert(bool value, const char *msg) {
  if (!value) {
    sprintf(buff, "[ASSERTION] %s", msg);
    ul_logger_erro(buff);
    ul_exit(1);
  }
}