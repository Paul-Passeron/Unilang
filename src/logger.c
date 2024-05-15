#include "../include/logger.h"
#include "../include/ul_compiler_globals.h"
#include <stdio.h>
#include <stdlib.h>

void create_logger(logger_t *logger) {
  logger->severity = SEV_INFO;
  logger->output = stdout;
}

void set_logger_severity(logger_t *logger, logger_severity_t severity) {
  logger->severity = severity;
}

void destroy_logger(logger_t logger) {
  FILE *f = logger.output;
  if (f == stdout)
    return;
  if (f == stderr)
    return;
  fclose(f);
}

void logger_log_with_severity(logger_t logger, const char *str,
                              logger_severity_t severity) {
  if (logger.severity > severity)
    return;
  switch (severity) {
  case SEV_INFO: {
    fprintf(logger.output, "\033[0;32m[INFO]\033[0m");
  } break;
  case SEV_WARN: {
    fprintf(logger.output, "\033[0;35m[WARNING]\033[0m");
  } break;
  case SEV_ERRO: {
    fprintf(logger.output, "\033[0;31m[ERROR]\033[0m");
  } break;
  default: // unreachable
    return;
  }
  fprintf(logger.output, " %s\n", str);
  fflush(logger.output);
}

void logger_info(logger_t logger, const char *str) {
  logger_log_with_severity(logger, str, SEV_INFO);
}

void logger_warn(logger_t logger, const char *str) {
  logger_log_with_severity(logger, str, SEV_WARN);
}

void logger_erro(logger_t logger, const char *str) {
  logger_log_with_severity(logger, str, SEV_ERRO);
}

void logger_log_with_severity_location(logger_t logger, location_t loc,
                                       const char *str,
                                       logger_severity_t severity) {
  if (logger.severity > severity)
    return;
  switch (severity) {
  case SEV_INFO: {
    fprintf(logger.output, "\033[0;32m[INFO]\033[0m");
  } break;
  case SEV_WARN: {
    fprintf(logger.output, "\033[0;35m[WARNING]\033[0m");
  } break;
  case SEV_ERRO: {
    fprintf(logger.output, "\033[0;31m[ERROR]\033[0m");
  } break;
  default: // unreachable
    return;
  }
  fprintf(logger.output, " ");
  print_location(logger.output, loc);
  fprintf(logger.output, " %s\n", str);
  fflush(logger.output);
}

void logger_info_location(logger_t logger, location_t loc, const char *str) {
  logger_log_with_severity_location(logger, loc, str, SEV_INFO);
}

void logger_warn_location(logger_t logger, location_t loc, const char *str) {
  logger_log_with_severity_location(logger, loc, str, SEV_WARN);
}

void logger_erro_location(logger_t logger, location_t loc, const char *str) {
  logger_log_with_severity_location(logger, loc, str, SEV_ERRO);
}

void set_logger_output_file(logger_t *logger, const char *path) {
  FILE *f = fopen(path, "wb");
  if (!f) {
    fprintf(stderr, "Could not open file '%s'\n", path);
    exit(1);
  }
  logger->output = f;
}

void set_logger_output(logger_t *logger, FILE *out) { logger->output = out; }

void ul_logger_info(const char *str) { logger_info(ul_global_logger, str); }

void ul_logger_warn(const char *str) { logger_warn(ul_global_logger, str); }

void ul_logger_erro(const char *str) { logger_erro(ul_global_logger, str); }

void ul_logger_info_location(location_t loc, const char *str) {
  logger_info_location(ul_global_logger, loc, str);
}

void ul_logger_warn_location(location_t loc, const char *str) {
  logger_warn_location(ul_global_logger, loc, str);
}

void ul_logger_erro_location(location_t loc, const char *str) {
  logger_erro_location(ul_global_logger, loc, str);
}

void ul_set_logger_output_file(const char *path) {
  set_logger_output_file(&ul_global_logger, path);
}

void ul_set_logger_output(FILE *out) {
  set_logger_output(&ul_global_logger, out);
}

void ul_destroy_logger(void) { destroy_logger(ul_global_logger); }
