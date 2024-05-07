#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include "location.h"

typedef enum logger_severity_t {
  SEV_INFO,
  SEV_WARN,
  SEV_ERRO,
} logger_severity_t;

typedef struct logger_t {
  logger_severity_t severity;
  FILE *output;
} logger_t;

void create_logger(logger_t *logger);

void set_logger_severity(logger_t *logger, logger_severity_t severity);

void destroy_logger(logger_t logger);

void logger_info(logger_t logger, const char *str);

void logger_warn(logger_t logger, const char *str);

void logger_erro(logger_t logger, const char *str);

void logger_info_location(logger_t logger, location_t loc, const char *str);

void logger_warn_location(logger_t logger, location_t loc, const char *str);

void logger_erro_location(logger_t logger, location_t loc, const char *str);

void set_logger_output_file(logger_t *logger, const char *path);

void set_logger_output(logger_t *logger, FILE *out);

#endif // LOGGER_H