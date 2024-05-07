#ifndef LOCATION_H
#define LOCATION_H

#include <stdio.h>

typedef struct location_t {
  size_t line;
  size_t col;
  const char *filename;
} location_t;

void print_location(FILE *f, location_t loc);

#endif // LOCATION_H