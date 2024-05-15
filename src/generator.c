// GENERATOR SOURCE FILE
// Paul Passeron

#include "../include/generator.h"
#include "../include/logger.h"
#include "../include/ul_assert.h"
#include "../include/ul_flow.h"

generator_t generator;

void set_generator_target(const char *target) {
  FILE *f = fopen(target, "w");
  if (f == NULL) {
    ul_logger_erro("Could not set generator to target...");
    ul_exit(1);
  }
  generator.target = f;
}

void destroy_generator(void) { fclose(generator.target); }

void generate_program(ast_t prog) {
  ast_array_t contents = prog->as.prog->prog;
  for (size_t i = 0; i < ul_dyn_length(contents); i++) {
    ast_t stmt = dyn_ast_get(contents, i);
    generate_statement(stmt);
  }
}

void generate_statement(ast_t stmt) {
  (void)stmt;
  ul_assert(false, "generate_statement is not implemented yet !");
}