#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

typedef char i8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef short i16;
typedef unsigned int u32;
typedef int i32;
typedef unsigned long u64;
typedef long i64;
typedef char *cstr;

void __UL_exit(u8 exit_code);

/**
LOGGER
*/

typedef enum logger_severity_t {
  SEV_INFO,
  SEV_WARN,
  SEV_ERRO,
  SEV_SILENT
} logger_severity_t;

typedef struct logger_t {
  logger_severity_t severity;
  FILE *output;
} logger_t;

logger_t ul_global_logger;

void create_logger(logger_t *logger) {
  logger->severity = SEV_INFO;
  logger->output = stdout;
}

void set_logger_severity(logger_t *logger, logger_severity_t severity) {
  logger->severity = severity;
}

void destroy_logger(logger_t logger) {
  FILE *f = logger.output;
  if (f == stdout)
    return;
  if (f == stderr)
    return;
  fclose(f);
}

void logger_log_with_severity(logger_t logger, const char *str,
                              logger_severity_t severity) {
  if (logger.severity > severity)
    return;
  switch (severity) {
  case SEV_INFO: {
    fprintf(logger.output, "\033[0;32m[INFO]\033[0m");
  } break;
  case SEV_WARN: {
    fprintf(logger.output, "\033[0;35m[WARNING]\033[0m");
  } break;
  case SEV_ERRO: {
    fprintf(logger.output, "\033[0;31m[ERROR]\033[0m");
  } break;
  default: // unreachable
    return;
  }
  fprintf(logger.output, " %s\n", str);
  fflush(logger.output);
}

void logger_info(logger_t logger, const char *str) {
  logger_log_with_severity(logger, str, SEV_INFO);
}

void logger_warn(logger_t logger, const char *str) {
  logger_log_with_severity(logger, str, SEV_WARN);
}

void logger_erro(logger_t logger, const char *str) {
  logger_log_with_severity(logger, str, SEV_ERRO);
}

void set_logger_output_file(logger_t *logger, const char *path) {
  FILE *f = fopen(path, "wb");
  if (!f) {
    fprintf(stderr, "Could not open file '%s'\n", path);
    __UL_exit(1);
  }
  logger->output = f;
}

void set_logger_output(logger_t *logger, FILE *out) { logger->output = out; }

void ul_logger_info(const char *str) { logger_info(ul_global_logger, str); }

void ul_logger_warn(const char *str) { logger_warn(ul_global_logger, str); }

void ul_logger_erro(const char *str) { logger_erro(ul_global_logger, str); }

void ul_set_logger_output_file(const char *path) {
  set_logger_output_file(&ul_global_logger, path);
}

void ul_set_logger_output(FILE *out) {
  set_logger_output(&ul_global_logger, out);
}

void ul_destroy_logger(void) { destroy_logger(ul_global_logger); }

//

/* ASSERT */

static char buff[1024];

void ul_assert(bool value, const char *msg) {
  if (!value) {
    sprintf(buff, "\033[0;31m[ASSERTION]\033[0m %s", msg);
    ul_logger_erro(buff);
    __UL_exit(1);
  }
}

/*ARENAS*/

typedef struct arena_t {
  size_t size;
  size_t fill;
  void *contents;
} arena_t;

#define MAX_ARENAS_NUM (1024 * 256)

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
  // printf("ID IS %d\n", id);
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
  ul_assert(__internal_arenas[__internal_current_arena].fill + s <=
                __internal_arenas[__internal_current_arena].size,
            "Could not allocate: Arena can't grow and s can't fit more alloc");
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
typedef struct __ul_internal_string *string;

string __internal_cstr_to_string(const char *contents);
char *__UL_string_to_cstr(string s);
string __UL_new_string(u32 count);
string __UL_char_to_string(char c);
string __UL_append_string(string dest, string to_append);

void __UL_entry();

void __UL_exit(u8 exit_code) {
  clear_allocator();
  // ul_destroy_logger();
}

#define __UL_syscall1(num, arg) syscall(num, arg)
#define __UL_syscall2(num, arg1, arg2) syscall(num, arg1, arg2)
#define __UL_syscall3(num, arg1, arg2, arg3) syscall(num, arg1, arg2, arg3)
#define __UL_syscall4(num, arg1, arg2, arg3, arg4)                             \
  syscall(num, arg1, arg2, arg3, arg4)
#define __UL_syscall5(num, arg1, arg2, arg3, arg4, arg5)                       \
  syscall(num, arg1, arg2, arg3, arg4, arg5)

int main(void) {
  unsigned int default_arena = new_arena(1);
  set_arena(default_arena);
  create_logger(&ul_global_logger);
  __UL_entry();
  __UL_exit(0);
  return 0; // Not needed but used so that all compilers are happy
  // O_TRUNC;
}

typedef struct __internal_array_t *__internal_array_t;
struct __internal_array_t {
  void *contents;
  size_t capacity;
  size_t length;
  size_t stride;
  unsigned int arena;
  unsigned int content_arena;
  bool is_ptr;
};

#define ARR_MIN_CAP 16
#define __internal_new_array(type, is_ptr) new_array(sizeof(type), is_ptr)
__internal_array_t new_array(size_t stride, bool is_ptr);
void array_t_append_raw(__internal_array_t arr, ...);

#define __UL___internal___internal_array_t_append(elem, arr)                   \
  array_t_append_raw(arr, elem);

void resize_arr(__internal_array_t arr) {
  size_t new_cap = 2 * arr->capacity;
  unsigned int old_arena = get_arena();
  unsigned int arena = new_arena(new_cap * arr->stride);
  set_arena(arena);
  void *new_contents = alloc(new_cap * arr->stride, 1);
  set_arena(old_arena);
  memcpy(new_contents, arr->contents, arr->stride * arr->capacity);
  destroy_arena(arr->content_arena);
  arr->content_arena = arena;
  arr->capacity = new_cap;
  arr->contents = new_contents;
}

typedef struct {
  i64 dummy;
} __internal_array_t_cast_t;

void array_t_append_raw(__internal_array_t arr, ...) {
  if (arr->length >= arr->capacity) {
    resize_arr(arr);
  }
  va_list ptr;
  va_start(ptr, arr);
  if (arr->is_ptr) {
    void *tmp = va_arg(ptr, void *);
    ((void **)arr->contents)[arr->length] = tmp;
  } else {
    __internal_array_t_cast_t tmp = va_arg(ptr, __internal_array_t_cast_t);
    for (size_t i = 0; i < arr->stride; i++) {
      ((char *)arr->contents)[i + arr->stride * arr->length] =
          ((char *)(&tmp))[i];
    }
  }
  arr->length++;
  va_end(ptr);
}

void array_t_set_raw(__internal_array_t arr, size_t index, ...) {

  va_list ptr;
  va_start(ptr, index);
  if (arr->is_ptr) {
    void *tmp = va_arg(ptr, void *);
    va_end(ptr);
    if (index >= arr->length)
      return;
    ((void **)arr->contents)[arr->length] = tmp;
  } else {
    __internal_array_t_cast_t tmp = va_arg(ptr, __internal_array_t_cast_t);
    va_end(ptr);
    if (index >= arr->length)
      return;
    for (size_t i = 0; i < arr->stride; i++) {
      ((char *)arr->contents)[i + arr->stride * index] = ((char *)(&tmp))[i];
    }
  }
}

#define __UL___internal___internal_array_t_set(index, elem, arr)               \
  array_t_set_raw(arr, index, elem)

__internal_array_t new_array(size_t stride, bool is_ptr) {
  unsigned int old_arena = get_arena();
  unsigned int arena = new_arena(sizeof(struct __internal_array_t));
  set_arena(arena);
  __internal_array_t arr = alloc(sizeof(struct __internal_array_t), 1);
  arr->arena = arena;
  arr->stride = stride;
  arr->is_ptr = is_ptr;
  arr->content_arena = new_arena(ARR_MIN_CAP * stride);
  set_arena(arr->content_arena);
  arr->contents = alloc(ARR_MIN_CAP * stride, 1);
  arr->capacity = ARR_MIN_CAP;
  arr->length = 0;
  set_arena(old_arena);
  return arr;
}

#define __UL___internal___internal_array_t_get(index, arr, type)               \
  *((type *)(array_get(index, arr)))

#define __UL___internal___internal_array_t_length(arr) arr->length

void *array_get(size_t index, __internal_array_t arr) {
  if (index > arr->length)
    return NULL;
  return (char *)arr->contents + arr->stride * index;
}

void __UL___internal___internal_array_t_swap(size_t i, size_t j,
                                             __internal_array_t arr) {
  char *contents[sizeof(__internal_array_t_cast_t)] = {0};
  memcpy(contents, arr->contents + i * arr->stride, arr->stride);
  memcpy(arr->contents + i * arr->stride, arr->contents + j * arr->stride,
         arr->stride);
  memcpy(arr->contents + j * arr->stride, contents, arr->stride);
}