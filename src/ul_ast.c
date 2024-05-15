// UL_AST SOURCE FILE
// Paul Passeron

#include "../include/ul_ast.h"
#include "../include/ul_allocator.h"
#include "../include/ul_assert.h"

extern unsigned int parser_arena;

ast_t new_strlit(char *content) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_str_lit_t *strlit = alloc(sizeof(ast_str_lit_t), 1);
  set_arena(old_arena);
  strlit->content = content;
  res->kind = A_STRLIT;
  res->as.strlit = strlit;
  return res;
}

ast_t new_charlit(char content) {
  (void)content;
  ul_assert(false, "new_charlit not implemented yet !");
  return NULL; //
}

ast_t new_numlit(char *content, bool has_point) {
  (void)content;
  (void)has_point;
  ul_assert(false, "new_numlit not implemented yet !");
  return NULL; //
}

ast_t new_binop(token_kind_t op, ast_t left, ast_t right) {
  (void)op;
  (void)left;
  (void)right;
  ul_assert(false, "new_binop not implemented yet !");
  return NULL; //
}

ast_t new_unary(token_kind_t op, ast_t operand, bool is_postfix) {
  (void)op;
  (void)operand;
  (void)is_postfix;
  ul_assert(false, "new_unary not implemented yet !");
  return NULL; //
}

ast_t new_iden(char *content) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_iden_t *iden = alloc(sizeof(ast_iden_t), 1);
  set_arena(old_arena);
  iden->content = content;
  res->kind = A_IDEN;
  res->as.iden = iden;
  return res;
}

ast_t new_prog() {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_prog_t *prog = alloc(sizeof(ast_prog_t), 1);
  set_arena(old_arena);
  ast_array_t arr = new_ast_dyn();
  *prog = (ast_prog_t){arr};
  *res = (struct ast_struct_t){A_PROG, {.prog = prog}};
  return res;
}

ast_t new_fundef_param(ast_t type, char *name) {
  (void)type;
  (void)name;
  ul_assert(false, "new_fundef_param not implemented yet !");
  return NULL;
}

ast_t new_fundef(ast_array_t params, ast_t return_type, char *name,
                 ast_array_t body) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_fundef_t *fundef = alloc(sizeof(ast_fundef_t), 1);
  set_arena(old_arena);
  fundef->params = params;
  fundef->return_type = return_type;
  fundef->name = name;
  fundef->body = body;
  res->kind = A_FUNDEF;
  res->as.fundef = fundef;
  return res;
}

ast_t new_funcall(char *name, ast_array_t args) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_funcall_t *funcall = alloc(sizeof(ast_funcall_t), 1);
  set_arena(old_arena);
  funcall->name = name;
  funcall->args = args;
  res->kind = A_FUNCALL;
  res->as.funcall = funcall;
  return res;
}

ast_t new_compound(ast_array_t stmts) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_compound_t *compound = alloc(sizeof(ast_compound_t), 1);
  set_arena(old_arena);
  *compound = (ast_compound_t){stmts};
  *res = (struct ast_struct_t){A_PROG, {.compound = compound}};
  return res;
}

ast_t new_index(ast_t value, ast_t index) {
  (void)value;
  (void)index;
  ul_assert(false, "new_index is not implemented yet !\n");
  return NULL;
}