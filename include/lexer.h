#ifndef LEXER_H
#define LEXER_H

#include "location.h"
#include "ul_dyn_arrays.h"
#include <stdbool.h>

typedef enum lexer_state_t {
  LS_DEFAULT,
  LS_SINGLE_LINE_COMMENT,
  LS_MULTI_LINE_COMMENT,
  LS_STRING_LIT,
  LS_CHAR_LIT,
  LS_NUM_LIT,
  LS_WORD,
  LS_DELIMITER,
  LS_INCLUDE,
} lexer_state_t;

const char *lexer_state_to_str(lexer_state_t state);

typedef struct lexer_t {
  char *buffer;
  size_t buffer_length;
  size_t buffer_index;
  location_t current_loc;
  char *filename;
  lexer_state_t state;
  token_array_t toks;
  unsigned int arena;
} lexer_t;

void new_lexer(lexer_t *l, char *path);
void destroy_lexer(lexer_t l);
char consume_char(lexer_t *l);
char peek_char(lexer_t l);
char peek_n_chars(lexer_t l, size_t n);
bool is_end_of_file(lexer_t l);
bool step_lexer(lexer_t *l);
void lex_program(lexer_t *l);

#endif // LEXER_H