#include "../include/ul_io.h"
#include "../include/ul_allocator.h"
#include "../include/ul_compiler_globals.h"
#include "../include/ul_flow.h"
#include <stdio.h>

void read_file(const char *path, char **dst, size_t *length) {
  // Will use the current arena
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    char buffer[1024] = {0};
    sprintf(buffer, "Could not load file \'%s\'", path);
    ul_logger_erro(buffer);
    ul_exit(1);
  }
  fseek(f, 0, SEEK_END);
  *length = ftell(f);
  fseek(f, 0, SEEK_SET);
  *dst = alloc(*length + 1, 1);
  fread(*dst, 1, *length, f);
  (*dst)[*length] = 0;
  fclose(f);
}