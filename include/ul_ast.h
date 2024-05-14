// UL_AST HEADER FILE
// Paul Passeron

// File created by the new_file tool !

#ifndef UL_AST_H
#define UL_AST_H

#include "token.h"
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
  A_FUNDEF_PARAM,
  A_FUNDEF,
  A_FUNCALL,
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
  token_kind_t op; // kind of operation (should be the token kind)
  ast_t left;      // left operand
  ast_t right;     // right operand
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

// Function defenition parameter ast node
typedef struct ast_fundef_param_t {
  ast_t type; // should be an identifier maybe ?
  char *name; // maybe should convert to an identifier as well ?
} ast_fundef_param_t;

typedef struct ast_fundef_t {
  ast_array_t params; // array of asts that should be seen as ast_fundef_param_t
  ast_t return_type;  // the return type of the function
  char *name;         // the name of the function
  ast_array_t body;
} ast_fundef_t;

typedef struct ast_funcall_t {
  char *name;
  ast_array_t args;
} ast_funcall_t;

// union for all ast nodes
typedef union ast_as_t {
  ast_str_lit_t *strlit;
  ast_char_lit_t *charlit;
  ast_num_lit_t *numlit;
  ast_binop_t *binop;
  ast_unary_t *unary;
  ast_iden_t *iden;
  ast_prog_t *prog;
  ast_fundef_param_t *fundef_param;
  ast_fundef_t *fundef;
  ast_funcall_t *funcall;
} ast_as_t;

// Main ast struct
struct ast_struct_t {
  ast_kind_t kind;
  ast_as_t as;
};

ast_t new_strlit(char *content);
ast_t new_charlit(char content);
ast_t new_numlit(char *content, bool has_point);
ast_t new_binop(token_kind_t op, ast_t left, ast_t right);
ast_t new_unary(token_kind_t op, ast_t operand, bool is_postfix);
ast_t new_iden(char *content);
ast_t new_prog(); // empty
ast_t new_fundef_param(ast_t type, char *name);
ast_t new_fundef(ast_array_t params, ast_t return_type, char *name,
                 ast_array_t body);
ast_t new_funcall(char *name, ast_array_t args);

#endif // UL_AST_H
