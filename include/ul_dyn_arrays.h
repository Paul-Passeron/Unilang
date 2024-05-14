#ifndef UL_DYN_ARRAYS_H
#define UL_DYN_ARRAYS_H

#include <stddef.h>
#include <stdarg.h>
#include "token.h"

typedef struct __internal_dyn_array_t {
  size_t stride;
  size_t capacity;
  size_t length;
  void *contents;
  unsigned int arena;
} __internal_dyn_array_t;

#define MAX_ITEM_SIZE 1024

void ul_dyn_append(__internal_dyn_array_t *arr, ...);
void __internal_resize_dyn_array(__internal_dyn_array_t *arr);
void *__internal_dyn_get(__internal_dyn_array_t arr, size_t index);

#define ul_dyn_get(arr, index, type) (*(type *)__internal_dyn_get(arr, index))

__internal_dyn_array_t __internal_new_dyn_array(size_t size);

#define new_dyn(type) __internal_new_dyn_array(sizeof(type));

void ul_dyn_destroy(__internal_dyn_array_t arr);

size_t ul_dyn_length(__internal_dyn_array_t arr);

// TOKEN
typedef __internal_dyn_array_t token_array_t;
#define dyn_tok_get(arr, index) ul_dyn_get(arr, index, token_t)
#define new_tok_dyn() new_dyn(token_t)


// AST
typedef __internal_dyn_array_t ast_array_t;
#define dyn_ast_get(arr, index) ul_dyn_get(arr, index, ast_t)
#define new_ast_dyn() new_dyn(ast_t)

#endif // UL_DYN_ARRAYS_H