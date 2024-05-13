#ifndef TOKEN_H
#define TOKEN_H

#include "location.h"
#include <stdio.h>

typedef enum token_kind_t {
  T_IDEN,
  T_STRLIT,
  T_NUMLIT,
  T_CHARLIT,

  T_IF,
  T_WHILE,
  T_FOR,

  T_PLUS,
  T_MINUS,
  T_MULT,
  T_DIV,
  T_MODULO,
  T_GRTR,
  T_GRTREQ,
  T_LSSR,
  T_LSSR_EQ,
  T_AND,
  T_OR,
  T_NOT,

  T_BIGARR,
  T_SMALLARR,
  T_OPENPAREN,
  T_CLOSEPAREN,
  T_OPENBRA,
  T_CLOSEBRA,
  T_COLON,
  T_SEMICOLON,

} token_kind_t;

typedef struct token_t {
  token_kind_t kind;
  const char *lexeme;
  location_t location;
} token_t;

void print_token(FILE *f, token_t tok);

#endif // TOKEN_H