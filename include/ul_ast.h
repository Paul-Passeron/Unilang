// UL_AST HEADER FILE
// Paul Passeron

// File created by the new_file tool !

#ifndef UL_AST_H
#define UL_AST_H

#include "token.h"
#include "ul_dyn_arrays.h"
#include "ul_types.h"
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
  A_COMPOUND,
  A_IF,
  A_RETURN,
  A_WHILE,
  A_LOOP,
  A_VARDEF,
  A_INDEX,
  A_ACCESS,
  A_TDEF,
  A_ASSIGN,
  A_TYPE,
  A_ITER,
} ast_kind_t;

// Str lit ast node
typedef struct ast_str_lit_t {
  const char *content; // contents of the string
} ast_str_lit_t;

// Char lit ast node
typedef struct ast_char_lit_t {
  const char *content; // actually containing the character itself
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

typedef struct ast_compound_t {
  ast_array_t stmts;
} ast_compound_t;

typedef struct ast_if_t {
  ast_t condition;
  ast_t ifstmt;
  ast_t elsestmt;
} ast_if_t;

typedef struct ast_return_t {
  ast_t expr;
} ast_return_t;

typedef struct ast_while_t {
  ast_t condition;
  ast_t stmt;
} ast_while_t;

// loop i: 0 -> n => {}
typedef struct ast_loop_t {
  char *varname;
  ast_t init;
  ast_t end;
  ast_t stmt;
  bool strict;
} ast_loop_t;

typedef struct ast_vardef_t {
  char *name;
  ast_t type;
  ast_t value;
} ast_vardef_t;

typedef struct ast_index_t {
  ast_t value;
  ast_t index;
} ast_index_t;

typedef struct ast_tdef_t {
  type_t type;
} ast_tdef_t;

typedef struct ast_access_t {
  ast_t object;
  ast_t field;
} ast_access_t;

typedef struct ast_assign_t {
  ast_t expr;
  ast_t value;
} ast_assign_t;

typedef struct ast_type_t {
  char *name;
  int list_n;
} ast_type_t;

typedef struct ast_iter_t {
  ast_t var;
  ast_t itered;
  ast_t stmt;
} ast_iter_t;

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
  ast_compound_t *compound;
  ast_if_t *ifstmt;
  ast_return_t *retstmt;
  ast_while_t *whilestmt;
  ast_loop_t *loop;
  ast_vardef_t *vardef;
  ast_index_t *index;
  ast_tdef_t *tdef;
  ast_access_t *access;
  ast_assign_t *assign;
  ast_type_t *type;
  ast_iter_t *iter;
} ast_as_t;

// Main ast struct
struct ast_struct_t {
  ast_kind_t kind;
  ast_as_t as;
  location_t loc;
};

ast_t new_strlit(location_t loc, char *content);
ast_t new_charlit(location_t loc, char *content);
ast_t new_numlit(location_t loc, char *content, bool has_point);
ast_t new_binop(location_t loc, token_kind_t op, ast_t left, ast_t right);
ast_t new_unary(location_t loc, token_kind_t op, ast_t operand,
                bool is_postfix);
ast_t new_iden(location_t loc, char *content);
ast_t new_prog(location_t loc); // empty
ast_t new_fundef_param(location_t loc, ast_t type, char *name);
ast_t new_fundef(location_t loc, ast_array_t params, ast_t return_type,
                 char *name, ast_array_t body);
ast_t new_funcall(location_t loc, char *name, ast_array_t args);
ast_t new_compound(location_t loc, ast_array_t stmts);
ast_t new_if(location_t loc, ast_t condition, ast_t ifstmt, ast_t elsestmt);
ast_t new_return(location_t loc, ast_t expr);
ast_t new_while(location_t loc, ast_t condition, ast_t stmt);
ast_t new_loop(location_t loc, char *varname, ast_t init, ast_t end, ast_t stmt,
               bool strict);
ast_t new_vardef(location_t loc, char *name, ast_t type, ast_t value);
ast_t new_index(location_t loc, ast_t value, ast_t index);
ast_t new_access(location_t loc, ast_t object, ast_t field);
ast_t new_tdef(location_t loc, type_t type);
ast_t new_assignement(location_t loc, ast_t expr, ast_t value);
ast_t new_type(location_t loc, char *name, int list_n);
ast_t new_iter(location_t loc, ast_t var, ast_t itered, ast_t stmt);

const char *ast_kind_to_str(ast_kind_t kind);

#endif // UL_AST_H
