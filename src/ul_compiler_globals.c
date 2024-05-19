#include "../include/ul_compiler_globals.h"

logger_t ul_global_logger;

bool streq(const char *s1, const char *s2) {
  if (s1 == NULL && s2 == NULL)
    return true;
  if (s1 == NULL)
    return false;
  if (s2 == NULL)
    return false;
  while (*s1 && *s2) {
    if (*(s1++) != *(s2++))
      return false;
  }
  return *s1 == *s2;
}