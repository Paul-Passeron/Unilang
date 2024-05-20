#ifndef UL_DYN_ARRAYS_H
#define UL_DYN_ARRAYS_H

#include "token.h"
#include <stdarg.h>
#include <stddef.h>

typedef struct __internal_dyn_array_t {
  size_t stride;
  size_t capacity;
  size_t length;
  void *contents;
  int is_ptr;
  unsigned int arena;
} __internal_dyn_array_t;

#define MAX_ITEM_SIZE 1024

void ul_dyn_append(__internal_dyn_array_t *arr, ...);
void __internal_resize_dyn_array(__internal_dyn_array_t *arr);
void *__internal_dyn_get(__internal_dyn_array_t arr, size_t index);

#define ul_dyn_get(arr, index, type) (*(type *)__internal_dyn_get(arr, index))
#define ul_dyn_get_ptr(arr, index, type) (type) __internal_dyn_get(arr, index)

__internal_dyn_array_t __internal_new_dyn_array(size_t size, int is_ptr);

#define new_dyn(type, is_ptr) __internal_new_dyn_array(sizeof(type), is_ptr);

void ul_dyn_destroy(__internal_dyn_array_t arr);

size_t ul_dyn_length(__internal_dyn_array_t arr);

void ul_dyn_destroy_last(__internal_dyn_array_t *arr);

// TOKEN
typedef __internal_dyn_array_t token_array_t;
#define dyn_tok_get(arr, index) ul_dyn_get(arr, index, token_t)
#define new_tok_dyn() new_dyn(token_t, false)

// AST
typedef __internal_dyn_array_t ast_array_t;
#define dyn_ast_get(arr, index) ul_dyn_get(arr, index, ast_t)
#define new_ast_dyn() new_dyn(ast_t, true)

// NAME TABLE
#define nt_get(arr, index) ul_dyn_get(arr, index, nt_entry_t)
#define new_nt() new_dyn(nt_entry_t, false)
#define append_nt(nt, name, kind, ref) ul_dyn_append(nt, (name_table_t){.name = name, ?kind = kind, .ref = ref})

// String
typedef __internal_dyn_array_t str_array_t;
#define dyn_str_get(arr, index) ul_dyn_get(arr, index, char *)
#define new_str_dyn() new_dyn(char *, true)

// TYPE
typedef __internal_dyn_array_t type_array_t;
#define dyn_type_get(arr, index) ul_dyn_get(arr, index, type_t)
#define new_type_dyn() new_dyn(type_t, false)

#endif // UL_DYN_ARRAYS_H