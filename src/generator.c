// GENERATOR SOURCE FILE
// Paul Passeron

#include "../include/generator.h"
#include "../include/logger.h"
#include "../include/ul_assert.h"
#include "../include/ul_flow.h"
#include "../include/ul_io.h"

generator_t generator;

#define gprintf(...) fprintf(generator.target, __VA_ARGS__)

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
  generate_prolog();
  ast_array_t contents = prog->as.prog->prog;
  for (size_t i = 0; i < ul_dyn_length(contents); i++) {
    ast_t *stmt_ptr = ((ast_t *)__internal_dyn_get(contents, i));
    fflush(stdout);
    generate_statement(*stmt_ptr);
  }
}

void generate_type(ast_t type) {
  if (type->kind == A_IDEN) {
    gprintf("%s", type->as.iden->content);
  }
}

void generate_fundef(ast_t fundef) {
  ast_fundef_t f = *fundef->as.fundef;
  generate_type(f.return_type);
  gprintf(" %s(", f.name);
  gprintf("){\n");
  for (size_t i = 0; i < ul_dyn_length(f.body); ++i) {
    ast_t stmt = dyn_ast_get(f.body, i);
    generate_statement(stmt);
    gprintf("\n");
  }
  gprintf("}\n");
}

// void generate_fundef_param(ast_t funcall) {}

void generate_funcall(ast_t funcall) {
  ast_funcall_t f = *funcall->as.funcall;
  gprintf("%s(", f.name);
  for (size_t i = 0; i < ul_dyn_length(f.args); ++i) {
    if (i > 0) {
      gprintf(", ");
    }
    ast_t arg = dyn_ast_get(f.args, i);
    generate_expression(arg);
  }
  gprintf(")");
}

void generate_expression(ast_t stmt) {
  if (stmt->kind == A_STRLIT) {
    gprintf("%s", stmt->as.strlit->content);
    return;
  }
  if (stmt->kind == A_FUNCALL) {
    generate_funcall(stmt);
    return;
  }
  ul_assert(false, "This kind of expression is not implemented yet");
}

void generate_statement(ast_t stmt) {
  if (stmt->kind == A_FUNDEF) {
    generate_fundef(stmt);
    return;
  }
  generate_expression(stmt);
  gprintf(";\n");
}

void generate_prolog() {
  size_t length;
  char *buff;
  read_file("src/template/prologue.c", &buff, &length);
  gprintf("%s", buff);
}