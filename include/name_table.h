// NAME_TABLE HEADER FILE
// Paul Passeron

// File created by the new_file tool !

#ifndef NAME_TABLE_H
#define NAME_TABLE_H

#include "location.h"
#include "ul_ast.h"
#include "ul_dyn_arrays.h"

typedef enum nt_entry_kind_t {
  NT_VAR,
  NT_FUNC,
  NT_TYPE,
} nt_entry_kind_t;

typedef struct nt_entry_t {
  char *name;
  nt_entry_kind_t kind;
  ast_t ref;
  // location_t loc; // maybe ?
} nt_entry_t;

typedef __internal_dyn_array_t nt_t;

#endif // NAME_TABLE_H
