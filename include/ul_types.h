// UL_TYPES HEADER FILE
// Paul Passeron

// File created by the new_file tool !

#ifndef UL_TYPES_H
#define UL_TYPES_H

#include "ul_dyn_arrays.h"
#include <stdbool.h>

typedef enum type_kind_t {
  TY_STRUCT,
  TY_ENUM,
  TY_PRIMITIVE,
} type_kind_t;

typedef struct type_t {
  char name[128];
  type_kind_t kind;
  str_array_t members_types; // Don't initialize for primitive types
  str_array_t members_names; // Don't initialize for primitive types
  bool is_builtin;
  bool is_signed;
  size_t size;
} type_t;

#endif // UL_TYPES_H
