#include "../include/ul_allocator.h"
#include "../include/ul_assert.h"

arena_t *__internal_arena_ptr = NULL;
arena_t __internal_arena;

#include <stdlib.h>

void new_arena(size_t size, bool should_grow) {
  if (__internal_arena_ptr != NULL) {
    destroy_arena();
  }
  void *contents = malloc(size);
  ul_assert(contents != NULL, "new_arena: Could not allocate contents");
  __internal_arena = (arena_t){.size = size,
                               .should_grow = should_grow,
                               .fill = 0,
                               .contents = contents};
  __internal_arena_ptr = &__internal_arena;
}

void destroy_arena(void) {
  ul_assert(__internal_arena_ptr != NULL,
            "destroy_arena: Cannot destroy arena: No arena found");
  free(__internal_arena.contents);
  __internal_arena = (arena_t){0};
  __internal_arena_ptr = NULL;
}

void *__internal_alloc(size_t s) {
  ul_assert(__internal_arena_ptr != NULL, "Could not allocate: No arena found");
  ul_assert(__internal_arena.fill + s < __internal_arena.size ||
                __internal_arena.should_grow,
            "Could not allocate: Arena can't grow and s can't fit more alloc");
  if (__internal_arena.fill + s >= __internal_arena.size) {
    // realloc
    ul_assert(false, "__internal_alloc: realloc not implemented yet for arenas "
                     "that can grow");
  }
  void *res = (char *)__internal_arena.contents + __internal_arena.fill;
  __internal_arena.fill += s;
  return res;
}

void *alloc_preset(size_t n, size_t s, char value) {
  ul_assert(n > 0, "alloc_preset: n should be > 0");
  ul_assert(s > 0, "alloc_preset: s should be > 0");
  void *res = __internal_alloc(n * s);
  memset(res, value, n * s);
  return res;
}
void *alloc_preset_ptr(size_t n, size_t s, char *value) {
  ul_assert(n > 0, "alloc_preset_ptr: n should be > 0");
  ul_assert(s > 0, "alloc_preset_ptr: s should be > 0");
  void *res = __internal_alloc(n * s);
  for (size_t i = 0; i < s; i++) {
    for (size_t j = 0; j < n; j++) {
      ((char *)res)[i + j * s] = value[i];
    }
  }
  return res;
}

void *alloc_zero(size_t n, size_t s) {
  ul_assert(n > 0, "alloc_zero: n should be > 0");
  ul_assert(s > 0, "alloc_zero: s should be > 0");
  return alloc_preset(n, s, 0);
}

void *alloc(size_t n, size_t s) {
  ul_assert(n > 0, "alloc: n should be > 0");
  ul_assert(s > 0, "alloc: s should be > 0");
  return __internal_alloc(s * n);
}
