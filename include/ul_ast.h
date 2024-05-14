// UL_AST HEADER FILE
// Paul Passeron

// File created by the new_file tool !

#ifndef UL_AST_H
#define UL_AST_H

#include "ul_dyn_arrays.h"
#include <stdbool.h>

// Main ast type
typedef struct ast_struct_t *ast_t;

// ast kinds
typedef enum ast_kind_t {
  A_STRLIT,
  A_CHARLIT,
  A_NUMLIT,
  A_BINOP,
  A_UNARY,
  A_IDEN,
  A_PROG,
} ast_kind_t;

// Str lit ast node
typedef struct ast_str_lit_t {
  const char *content; // contents of the string
} ast_str_lit_t;

// Char lit ast node
typedef struct ast_char_lit_t {
  char content; // actually containing the character itself
} ast_char_lit_t;

// Num lit ast node
typedef struct ast_num_lit_t {
  char *content;  // content as a string (lexeme of token) of the literal
  bool has_point; // whether or not the numlit contains a point (is float
                  // basically)
} ast_num_lit_t;

// Binary operation ast node
typedef struct ast_binop_t {
  int op;      // kind of operation (should be the token kind)
  ast_t left;  // left operand
  ast_t right; // right operand
} ast_binop_t;

// Unary operation ast node
typedef struct ast_unary_t {
  int op;
  ast_t operand;
  bool is_postfix; // whether or not we have a prefix (!a) or a postfix (a++)
                   // operation
} ast_unary_t;

// Identifier ast node
typedef struct ast_iden_t {
  char *content; // the name of the identifier (the token lexeme)
} ast_iden_t;

// Program ast node
typedef struct ast_prog_t {
  ast_array_t prog; // dynamic array of ast nodes that represent the program
} ast_prog_t;

// union for all ast nodes
typedef union ast_as_t {
  ast_str_lit_t *strlit;
  ast_char_lit_t *charlit;
  ast_num_lit_t *numlit;
  ast_binop_t *binop;
  ast_unary_t *unary;
  ast_iden_t *iden;
  ast_prog_t *prog;
} ast_as_t;

// Main ast struct
struct ast_struct_t {
  ast_kind_t kind;
  ast_as_t as;
};

#endif // UL_AST_H
