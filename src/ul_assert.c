#include "../include/ul_assert.h"
#include "../include/logger.h"
#include "../include/ul_flow.h"

static char buff[1024];

void ul_assert(bool value, const char *msg) {
  if (!value) {
    sprintf(buff, "\033[0;31m[ASSERTION]\033[0m %s", msg);
    ul_logger_erro(buff);
    ul_exit(1);
  }
}

void ul_assert_location(location_t loc, bool value, const char *msg) {
  if (!value) {
    sprintf(buff, "\033[0;31m[ASSERTION]\033[0m %s", msg);
    ul_logger_erro_location(loc, buff);
    ul_exit(1);
  }
}
