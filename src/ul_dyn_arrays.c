#include "../include/ul_dyn_arrays.h"
#include "../include/ul_allocator.h"
#include <stdarg.h>
#include <string.h>

#define __INTERNAL_DYN_ARRAY_INIT_CAP 16

__internal_dyn_array_t __internal_new_dyn_array(size_t size) {
  __internal_dyn_array_t res;
  res.stride = size;
  res.length = 0;
  res.capacity = __INTERNAL_DYN_ARRAY_INIT_CAP * res.stride;
  unsigned int previous_arena = get_arena();
  res.arena = new_arena(res.capacity);
  set_arena(res.arena);
  res.contents = alloc(res.capacity, 1);
  set_arena(previous_arena);

  return res;
}

void ul_dyn_destroy(__internal_dyn_array_t arr) { destroy_arena(arr.arena); }

typedef struct {
  char contents[MAX_ITEM_SIZE];
} __internal_default_t;

void ul_dyn_append(__internal_dyn_array_t *arr, ...) {
  va_list ptr;
  va_start(ptr, arr);

  unsigned int old_arena = get_arena();
  unsigned int a = new_arena(arr->stride);
  set_arena(a);
  char *buff = alloc(1, arr->stride);

  // We suppose there is only one variadic argument
  // maybe check for endianness ??
  __internal_default_t t = va_arg(ptr, __internal_default_t);
  for (size_t i = 0; i < arr->stride; ++i) {
    buff[i] = t.contents[i];
  }
  va_end(ptr);

  if (arr->length * arr->stride >= arr->capacity) {
    __internal_resize_dyn_array(arr);
  }

  memcpy((char *)arr->contents + arr->length * arr->stride, buff, arr->stride);
  arr->length++;

  destroy_arena(a);
  set_arena(old_arena);
}

void __internal_resize_dyn_array(__internal_dyn_array_t *arr) {
  size_t old_cap = arr->capacity;
  size_t new_cap = old_cap * 2;
  void *old_contents = arr->contents;
  unsigned int saved_arena = get_arena();
  unsigned int old_ar = arr->arena;
  unsigned int new_ar = new_arena(new_cap);
  set_arena(new_ar);
  void *new_contents = alloc(arr->capacity, 1);
  memcpy(new_contents, old_contents, old_cap);
  destroy_arena(old_ar);
  set_arena(saved_arena);
  arr->arena = new_ar;
  arr->contents = new_contents;
  arr->capacity = new_cap;
}

void *__internal_dyn_get(__internal_dyn_array_t arr, size_t index) {
  return (char *)arr.contents + index * arr.stride;
}

size_t ul_dyn_length(__internal_dyn_array_t arr) { return arr.length; }
