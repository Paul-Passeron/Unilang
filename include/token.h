#ifndef TOKEN_H
#define TOKEN_H

#include "location.h"
#include <stdio.h>

typedef enum token_kind_t {
  TOK_PLUS,

} token_kind_t;

typedef struct token_t {
  token_kind_t kind;
  const char *lexeme;
  location_t location;
} token_t;

void print_token(FILE *f, token_t tok);

#endif // TOKEN_H