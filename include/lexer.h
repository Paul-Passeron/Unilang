#ifndef LEXER_H
#define LEXER_H

#include "location.h"
#include <stdbool.h>

typedef enum lexer_state_t {
  LS_DEFAULT,
  LS_SINGLE_LINE_COMMENT,
  LS_MULTI_LINE_COMMENT,
} lexer_state_t;

typedef struct lexer_t {
  char *buffer;
  size_t buffer_length;
  size_t buffer_index;
  location_t current_loc;
  const char *filename;
  lexer_state_t state;
} lexer_t;

void new_lexer(lexer_t *l, const char *path);
char consume_char(lexer_t *l);
char peek_char(lexer_t l);
char peek_n_chars(lexer_t l, size_t n);
bool is_end_of_file(lexer_t l);

#endif // LEXER_H