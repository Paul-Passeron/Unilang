#include "../include/logger.h"
#include "../include/location.h"

int main(void) {
  logger_t logger;
  create_logger(&logger);
  logger_info(logger, "Started Unilang compiler");

  logger_info(logger, "Successfully ended Unilang compiler");
  destroy_logger(logger);
  return 0;
}