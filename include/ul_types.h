// UL_TYPES HEADER FILE
// Paul Passeron

// File created by the new_file tool !

#ifndef UL_TYPES_H
#define UL_TYPES_H

#include "ul_dyn_arrays.h"

typedef struct type_t {
  char *name;
  str_array_t members_types;
  str_array_t members_names;
} type_t;

#endif // UL_TYPES_H
