#ifndef TOKEN_H
#define TOKEN_H

#include "location.h"
#include <stdio.h>

typedef enum token_kind_t {
  T_STRLIT,
  T_NUMLIT,
  T_CHARLIT,
  T_WORD,
  T_PLUS,
  T_MINUS,
  T_MULT,
  T_DIV,
  T_MODULO,
  T_GRTR,
  T_GRTR_EQ,
  T_LSSR,
  T_LSSR_EQ,
  T_AND,
  T_OR,
  T_NOT,
  T_EQ,
  T_DIFF,
  T_BIGARR,
  T_SMALLARR,
  T_OPENPAREN,
  T_CLOSEPAREN,
  T_OPENBRACKET,
  T_OPENBRACE,
  T_CLOSEBRACKET,
  T_CLOSEBRACE,
  T_COLON,
  T_SEMICOLON,
  T_DOT,
  T_COMMA

} token_kind_t;

typedef struct token_t {
  token_kind_t kind;
  char *lexeme;
  location_t location;
} token_t;

const char *token_kind_to_str(token_kind_t t);

void print_token(token_t tok);

int get_precedence(token_kind_t t);

#endif // TOKEN_H