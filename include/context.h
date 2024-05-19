/**
 * context.h
 * Copyright (C) 2024 Paul Passeron
 * CONTEXT header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

// File created by the new_file tool !

#ifndef CONTEXT_H
#define CONTEXT_H

#include "ul_dyn_arrays.h"

typedef struct var_t {
  char name[128];
  char type[128];
} var_t;

typedef __internal_dyn_array_t var_array_t;
#define dyn_var_get(arr, index) ul_dyn_get(arr, index, var_t)
#define new_var_dyn() new_dyn(var_t, false)

typedef struct context_t {
  type_array_t types;
  var_array_t vars;
} context_t;

#endif // CONTEXT_H
