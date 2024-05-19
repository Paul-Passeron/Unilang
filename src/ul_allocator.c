#include "../include/ul_allocator.h"
#include "../include/ul_assert.h"
#include <stdlib.h>

#define MAX_ARENAS_NUM 1024

arena_t __internal_arenas[MAX_ARENAS_NUM] = {0};
bool __internal_arenas_popul[MAX_ARENAS_NUM] = {0};
int __internal_current_arena = -1;

int get_first_empty_id(void) {
  for (int i = 0; i < MAX_ARENAS_NUM; i++) {
    if (!__internal_arenas_popul[i])
      return i;
  }
  return -1;
}

unsigned int new_arena(size_t size) {
  int res = get_first_empty_id();
  ul_assert(res >= 0,
            "new_arena: Could not create arena... Max arena number exceeded");

  void *contents = malloc(size);
  ul_assert(contents != NULL, "new_arena: Could not allocate contents");
  arena_t tmp = {.size = size, .fill = 0, .contents = contents};
  __internal_arenas[res] = tmp;
  __internal_arenas_popul[res] = true;
  return res;
}

void destroy_arena(unsigned int id) {
  ul_assert(id < MAX_ARENAS_NUM, "destroy_arena: Max arena number reached");
  ul_assert(__internal_arenas_popul[id],
            "destroy_arena: Cannot destroy arena: No arena found");
  free(__internal_arenas[id].contents);
  __internal_arenas[id] = (arena_t){0};
  __internal_arenas_popul[id] = false;
}

void set_arena(unsigned int id) {
  ul_assert(__internal_arenas_popul[id], "set_arena: Invalid arena id.");
  __internal_current_arena = id;
}

unsigned int get_arena(void) { return __internal_current_arena; }

void clear_allocator(void) {
  for (int i = 0; i < MAX_ARENAS_NUM; i++) {
    if (__internal_arenas_popul[i])
      destroy_arena(i);
  }
}

void *__internal_alloc(size_t s) {
  ul_assert(__internal_current_arena >= 0,
            "Could not allocate: No arena found");
  ul_assert(__internal_arenas_popul[__internal_current_arena],
            "Could not allocate: No arena found");
  char msg[256];
  sprintf(msg,
          "Could not allocate: Arena can't grow and s can't fit more alloc. "
          "Size is %ld",
          __internal_arenas[__internal_current_arena].size);
  ul_assert(__internal_arenas[__internal_current_arena].fill + s <=
                __internal_arenas[__internal_current_arena].size,
            msg);
  void *res = (char *)__internal_arenas[__internal_current_arena].contents +
              __internal_arenas[__internal_current_arena].fill;
  __internal_arenas[__internal_current_arena].fill += s;
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
