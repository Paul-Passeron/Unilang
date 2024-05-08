#ifndef UL_ALLOCATOR_H
#define UL_ALLOCATOR_H

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

typedef struct arena_t {
  size_t size;
  bool should_grow;
  size_t fill;
  void *contents;
} arena_t;

unsigned int new_arena(size_t size, bool should_grow);
void destroy_arena(unsigned int id);
void set_arena(unsigned int id);
unsigned int get_arena(void);

void *alloc_preset(size_t n, size_t s, char value);
void *alloc_preset_ptr(size_t n, size_t s, char *value);
void *alloc_zero(size_t n, size_t s);
void *alloc(size_t n, size_t s);

#endif // UL_ALLOCATOR_H