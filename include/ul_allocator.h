#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef struct arena_t {
  size_t size;
  bool should_grow;
} arena_t;

void *alloc_preset(size_t n, size_t s, char value);
void *alloc_preset_ptr(size_t n, size_t s, char *value);
void *alloc_zero(size_t n, size_t s);
void *alloc(size_t n, size_t s);

#ifdef USE_LIBC_MALLOC
void free_ul(void *ptr);
#endif // USE_LIBC_MALLOC

#ifdef UL_ALLOC_IMPLEMENTATION

#ifdef USE_LIBC_MALLOC
#include <stdlib.h>
#include <string.h>

void *alloc_preset(size_t n, size_t s, char value) {
  void *res = malloc(s * n);
  ul_assert(res != NULL, "alloc_preset: Ran out of memory...");
  memset(res, value, s * n);
  return res;
}
void *alloc_preset_ptr(size_t n, size_t s, char *value) { //
  void *res = malloc(s * n);
  ul_assert(res != NULL, "alloc_preset_ptr: Ran out of memory...");
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < s; j++) {
      ((char *)res)[i * n + j] = value[j];
    }
  }
  return res;
}

void *alloc_zero(size_t n, size_t s) {
  void *res = malloc(s * n);
  ul_assert(res != NULL, "alloc_zero: Ran out of memory...");
  memset(res, 0, s * n);
  return res;
}

void *alloc(size_t n, size_t s) {
  void *res = malloc(s * n);
  ul_assert(res != NULL, "alloc_zero: Ran out of memory...");
  return res;
}

void free_ul(void *ptr) {
  ul_assert(ptr != NULL, "free_ul: Do not free NULL pointer.");
  free(ptr);
}

#else // ifdef USE_LIBC_MALLOC

// Custom implementation

void *alloc_preset(size_t n, size_t s, char value) {
  (void)s;
  (void)n;
  (void)value;
  ul_assert(false, "USE_CUSTOM_MALLOC alloc_preset is not implemented yet.");

  return NULL;
}
void *alloc_preset_ptr(size_t n, size_t s, char *value) {
  (void)s;
  (void)n;
  (void)value;
  ul_assert(false,
            "USE_CUSTOM_MALLOC alloc_preset_ptr is not implemented yet.");

  return NULL;
}
void *alloc_zero(size_t n, size_t s) {
  (void)s;
  (void)n;
  ul_assert(false, "USE_CUSTOM_MALLOC alloc_zero is not implemented yet.");

  return NULL;
}
void *alloc(size_t n, size_t s) {
  (void)s;
  (void)n;
  ul_assert(false, "USE_CUSTOM_MALLOC alloc is not implemented yet.");
  return NULL;
}

#endif // ifdef USE_LIBC_MALLOC
#endif // UL_ALLOC_IMPLEMENTATION

#endif // ALLOCATOR_H