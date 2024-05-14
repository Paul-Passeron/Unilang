// PARSER HEADER FILE
// Paul Passeron

// File created by the new_file tool !

#ifndef PARSER_H
#define PARSER_H

#include "../include/token.h"
#include "../include/ul_ast.h"
#include "../include/ul_dyn_arrays.h"
#include <stdbool.h>

typedef struct parser_t {
  token_array_t toks;
  size_t current_index;
  unsigned int arena;
} parser_t;

// returns a fresh parser
parser_t new_parser(token_array_t toks);

// returns if parser does not have any more current token to peek /
// consume
bool is_parser_done(parser_t p);

// returns the current token and returns a 0 initialized one if there is none
token_t peek_parser(parser_t p);

// asserts that current token is of kind kind
void expect(parser_t p, token_kind_t kind);

// asserts that current token's lexeme is equal lexeme
void expect_lexeme(parser_t p, const char *lexeme);

// returns the current token and avances to the next one
token_t consume_parser(parser_t *p);

// parses the program and returns the ast
ast_t parse_program(parser_t *p);

ast_t parse_statement(parser_t *p);
ast_t parse_expression(parser_t *p);
ast_t parse_strlit(parser_t *p);
ast_t parse_numlit(parser_t *p);
ast_t parse_charlit(parser_t *p);
ast_t parse_fundef(parser_t *p);
ast_t parse_fundef_param(parser_t *p);
ast_t parse_funcall(parser_t *p);
ast_t parse_identifier(parser_t *p);

#endif // PARSER_H
