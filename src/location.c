#include "../include/location.h"
#include <stdio.h>

void print_location(FILE *f, location_t loc) {
  fprintf(f, "%s:%ld:%ld", loc.filename, loc.line, loc.col);
}