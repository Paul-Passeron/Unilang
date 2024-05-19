// UL_AST SOURCE FILE
// Paul Passeron

#include "../include/ul_ast.h"
#include "../include/ul_allocator.h"
#include "../include/ul_assert.h"

extern unsigned int parser_arena;

ast_t new_strlit(location_t loc, char *content) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_str_lit_t *strlit = alloc(sizeof(ast_str_lit_t), 1);
  set_arena(old_arena);
  strlit->content = content;
  res->kind = A_STRLIT;
  res->as.strlit = strlit;
  res->loc = loc;
  return res;
}

ast_t new_charlit(location_t loc, char *content) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_char_lit_t *charlit = alloc(sizeof(ast_char_lit_t), 1);
  set_arena(old_arena);
  charlit->content = content;
  res->kind = A_CHARLIT;
  res->as.charlit = charlit;
  res->loc = loc;
  return res;
}

ast_t new_numlit(location_t loc, char *content, bool has_point) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_num_lit_t *nlit = alloc(sizeof(ast_num_lit_t), 1);
  set_arena(old_arena);
  nlit->content = content;
  nlit->has_point = has_point;
  res->kind = A_NUMLIT;
  res->as.numlit = nlit;
  res->loc = loc;
  return res;
}

ast_t new_binop(location_t loc, token_kind_t op, ast_t left, ast_t right) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_binop_t *binop = alloc(sizeof(ast_binop_t), 1);
  set_arena(old_arena);
  binop->op = op;
  binop->left = left;
  binop->right = right;
  res->kind = A_BINOP;
  res->as.binop = binop;
  res->loc = loc;
  return res;
}

ast_t new_unary(location_t loc, token_kind_t op, ast_t operand,
                bool is_postfix) {
  (void)op;
  (void)operand;
  (void)is_postfix;
  (void)loc;
  ul_assert(false, "new_unary not implemented yet !");
  return NULL; //
}

ast_t new_iden(location_t loc, char *content) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_iden_t *iden = alloc(sizeof(ast_iden_t), 1);
  set_arena(old_arena);
  iden->content = content;
  res->kind = A_IDEN;
  res->as.iden = iden;
  res->loc = loc;
  return res;
}

ast_t new_prog(location_t loc) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_prog_t *prog = alloc(sizeof(ast_prog_t), 1);
  set_arena(old_arena);
  ast_array_t arr = new_ast_dyn();
  *prog = (ast_prog_t){arr};
  *res = (struct ast_struct_t){A_PROG, {.prog = prog}, .loc = {0}};
  res->loc = loc;
  return res;
}

ast_t new_fundef_param(location_t loc, ast_t type, char *name) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_fundef_param_t *param = alloc(sizeof(ast_fundef_param_t), 1);
  set_arena(old_arena);
  param->name = name;
  param->type = type;
  res->kind = A_FUNDEF_PARAM;
  res->as.fundef_param = param;
  res->loc = loc;
  return res;
}

ast_t new_fundef(location_t loc, ast_array_t params, ast_t return_type,
                 char *name, ast_array_t body) {
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
  res->loc = loc;
  return res;
}

ast_t new_funcall(location_t loc, char *name, ast_array_t args) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_funcall_t *funcall = alloc(sizeof(ast_funcall_t), 1);
  set_arena(old_arena);
  funcall->name = name;
  funcall->args = args;
  res->kind = A_FUNCALL;
  res->as.funcall = funcall;
  res->loc = loc;
  return res;
}

ast_t new_compound(location_t loc, ast_array_t stmts) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_compound_t *compound = alloc(sizeof(ast_compound_t), 1);
  set_arena(old_arena);
  *compound = (ast_compound_t){stmts};
  *res = (struct ast_struct_t){A_COMPOUND, {.compound = compound}, .loc = {0}};
  res->loc = loc;
  return res;
}

ast_t new_index(location_t loc, ast_t value, ast_t index) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_index_t *i = alloc(sizeof(ast_index_t), 1);
  set_arena(old_arena);
  i->index = index;
  i->value = value;
  res->as.index = i;
  res->kind = A_INDEX;
  return res;
}

ast_t new_vardef(location_t loc, char *name, ast_t type, ast_t value) {
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
  res->loc = loc;
  return res;
}

ast_t new_if(location_t loc, ast_t condition, ast_t ifstmt, ast_t elsestmt) {
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
  res->loc = loc;
  return res;
}

ast_t new_return(location_t loc, ast_t expr) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_return_t *ret = alloc(sizeof(ast_return_t), 1);
  set_arena(old_arena);
  ret->expr = expr;
  res->kind = A_RETURN;
  res->as.retstmt = ret;
  res->loc = loc;
  return res;
}

ast_t new_loop(location_t loc, char *varname, ast_t init, ast_t end, ast_t stmt,
               bool strict) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_loop_t *loop = alloc(sizeof(ast_loop_t), 1);
  set_arena(old_arena);
  loop->end = end;
  loop->init = init;
  loop->varname = varname;
  loop->stmt = stmt;
  loop->strict = strict;
  res->kind = A_LOOP;
  res->as.loop = loop;
  res->loc = loc;
  return res;
}

ast_t new_access(location_t loc, ast_t object, ast_t field) {
  unsigned int old_arena = get_arena();
  set_arena(parser_arena);
  ast_t res = alloc(sizeof(struct ast_struct_t), 1);
  ast_access_t *access = alloc(sizeof(ast_access_t), 1);
  set_arena(old_arena);
  access->object = object;
  access->field = field;
  res->kind = A_ACCESS;
  res->as.access = access;
  res->loc = loc;
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
  case A_ACCESS:
    return "A_ACCESS";
  default:
    return "";
  }
}