#ifndef ULCOMPILER_GLOBALS_H
#define ULCOMPILER_GLOBALS_H

#include "logger.h"
#include <stdbool.h>

extern logger_t ul_global_logger;

bool streq(const char *s1, const char *s2);

#endif // ULCOMPILER_GLOBALS_H