#ifndef LEXER_H
#define LEXER_H

#include "location.h"

typedef enum lexer_state_t {
  LS_SINGLE_LINE_COMMENT,
} lexer_state_t;

typedef struct lexer_t {
  const char *buffer;
  size_t buffer_index;
  location_t current_loc;
  const char *filename;
} lexer_t;

#endif // LEXER_H