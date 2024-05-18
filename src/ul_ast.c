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
  unsigned int old_arena = get_arena();
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_num_lit_t *nlit = alloc(sizeof(ast_num_lit_t), 1);
  set_arena(old_arena);
  nlit->content = content;
  nlit->has_point = has_point;
  res->kind = A_NUMLIT;
  res->as.numlit = nlit;
  return res;
}

ast_t new_binop(token_kind_t op, ast_t left, ast_t right) {
  unsigned int old_arena = get_arena();
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_binop_t *binop = alloc(sizeof(ast_binop_t), 1);
  set_arena(old_arena);
  binop->op = op;
  binop->left = left;
  binop->right = right;
  res->kind = A_BINOP;
  res->as.binop = binop;
  return res;
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
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_fundef_param_t *param = alloc(sizeof(ast_fundef_param_t), 1);
  set_arena(old_arena);
  param->name = name;
  param->type = type;
  res->kind = A_FUNDEF_PARAM;
  res->as.fundef_param = param;
  return res;
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
  *res = (struct ast_struct_t){A_COMPOUND, {.compound = compound}};
  return res;
}

ast_t new_index(ast_t value, ast_t index) {
  (void)value;
  (void)index;
  ul_assert(false, "new_index is not implemented yet !\n");
  return NULL;
}

ast_t new_vardef(char *name, ast_t type, ast_t value) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_vardef_t *vdef = alloc(sizeof(ast_vardef_t), 1);
  set_arena(old_arena);
  vdef->name = name;
  vdef->type = type;
  vdef->value = value;
  res->kind = A_VARDEF;
  res->as.vardef = vdef;
  return res;
}

ast_t new_if(ast_t condition, ast_t ifstmt, ast_t elsestmt) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_if_t *ifnode = alloc(sizeof(ast_if_t), 1);
  set_arena(old_arena);
  ifnode->condition = condition;
  ifnode->ifstmt = ifstmt;
  ifnode->elsestmt = elsestmt;
  res->kind = A_IF;
  res->as.ifstmt = ifnode;
  return res;
}

const char *ast_kind_to_str(ast_kind_t kind) {
  switch (kind) {
  case A_STRLIT:
    return "A_STRLIT";
  case A_CHARLIT:
    return "A_CHARLIT";
  case A_NUMLIT:
    return "A_NUMLIT";
  case A_BINOP:
    return "A_BINOP";
  case A_UNARY:
    return "A_UNARY";
  case A_IDEN:
    return "A_IDEN";
  case A_PROG:
    return "A_PROG";
  case A_FUNDEF_PARAM:
    return "A_FUNDEF_PARAM";
  case A_FUNDEF:
    return "A_FUNDEF";
  case A_FUNCALL:
    return "A_FUNCALL";
  case A_COMPOUND:
    return "A_COMPOUND";
  case A_IF:
    return "A_IF";
  case A_RETURN:
    return "A_RETURN";
  case A_WHILE:
    return "A_WHILE";
  case A_LOOP:
    return "A_LOOP";
  case A_VARDEF:
    return "A_VARDEF";
  case A_INDEX:
    return "A_INDEX";
  default:
    return "";
  }
}