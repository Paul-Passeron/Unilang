// GENERATOR HEADER FILE
// Paul Passeron

// File created by the new_file tool !

#ifndef GENERATOR_H
#define GENERATOR_H
#include "ul_ast.h"
#include "ul_dyn_arrays.h"
#include "ul_types.h"
#include <stdio.h>

typedef struct generator_t {
  FILE *target;
  type_array_t types;
} generator_t;

void set_generator_target(const char *target);

void destroy_generator(void);

void generate_program(ast_t prog);
void generate_prolog();
void generate_statement(ast_t stmt);
void generate_expression(ast_t stmt);
void generate_expression_with_type(ast_t stmt, ast_t type);

#endif // GENERATOR_H
