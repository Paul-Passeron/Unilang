#include "../include/ul_compiler_globals.h"

logger_t ul_global_logger;
bool streq(const char *s1, const char *s2) {
  while (*s1 && *s2) {
    if (*(s1++) != *(s2++))
      return false;
  }
  return true;
}