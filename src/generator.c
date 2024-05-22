// GENERATOR SOURCE FILE
// Paul Passeron

#include "../include/generator.h"
#include "../include/logger.h"
#include "../include/ul_allocator.h"
#include "../include/ul_assert.h"
#include "../include/ul_compiler_globals.h"
#include "../include/ul_flow.h"
#include "../include/ul_io.h"
#include <fcntl.h>

generator_t generator;
ast_t program;

#define FUN_PREFIX "__UL_"
#define gprintf(...) fprintf(generator.target, __VA_ARGS__)

// Create builtin types

const type_t U8_TYPE = {.name = "u8",
                        .is_builtin = true,
                        .size = 1,
                        .kind = TY_PRIMITIVE,
                        .is_signed = false,
                        .list_n = 0};

const type_t BOOL_TYPE = {.name = "bool",
                          .is_builtin = true,
                          .size = 1,
                          .kind = TY_PRIMITIVE,
                          .is_signed = false,
                          .list_n = 0};

const type_t I8_TYPE = {.name = "i8",
                        .is_builtin = true,
                        .size = 1,
                        .kind = TY_PRIMITIVE,
                        .is_signed = true,
                        .list_n = 0};

const type_t CHAR_TYPE = {.name = "char",
                          .is_builtin = true,
                          .size = 1,
                          .kind = TY_PRIMITIVE,
                          .is_signed = true,
                          .list_n = 0};

const type_t U16_TYPE = {.name = "u16",
                         .is_builtin = true,
                         .size = 2,
                         .kind = TY_PRIMITIVE,
                         .is_signed = false,
                         .list_n = 0};

const type_t I16_TYPE = {.name = "i16",
                         .is_builtin = true,
                         .size = 2,
                         .kind = TY_PRIMITIVE,
                         .is_signed = true,
                         .list_n = 0};

const type_t U32_TYPE = {.name = "u32",
                         .is_builtin = true,
                         .size = 4,
                         .kind = TY_PRIMITIVE,
                         .is_signed = false,
                         .list_n = 0};

const type_t I32_TYPE = {.name = "i32",
                         .is_builtin = true,
                         .size = 4,
                         .kind = TY_PRIMITIVE,
                         .is_signed = true,
                         .list_n = 0};

const type_t U64_TYPE = {.name = "u64",
                         .is_builtin = true,
                         .size = 8,
                         .kind = TY_PRIMITIVE,
                         .is_signed = false,
                         .list_n = 0};

const type_t I64_TYPE = {.name = "i64",
                         .is_builtin = true,
                         .size = 8,
                         .kind = TY_PRIMITIVE,
                         .is_signed = true,
                         .list_n = 0};

const type_t VOID_TYPE = {.name = "void",
                          .is_builtin = true,
                          .size = 0,
                          .kind = TY_PRIMITIVE,
                          .is_signed = false,
                          .list_n = 0};

const type_t CSTR_TYPE = {.name = "cstr",
                          .is_builtin = true,
                          .size = 8,
                          .kind = TY_PRIMITIVE,
                          .is_signed = false,
                          .list_n = 0};

type_t ARR_TYPE = {.name = "__internal_array_t",
                   .is_builtin = true,
                   .size = 8,
                   .kind = TY_ARRAY,
                   .is_signed = false,
                   .list_n = 0};

void set_generator_target(const char *target) {
  FILE *f = fopen(target, "w");
  if (f == NULL) {
    ul_logger_erro("Could not set generator to target...");
    ul_exit(1);
  }
  generator.context.types = new_type_dyn();
  generator.context.vars = new_var_dyn();
  ul_dyn_append(&generator.context.types, CHAR_TYPE);
  ul_dyn_append(&generator.context.types, U8_TYPE);
  ul_dyn_append(&generator.context.types, BOOL_TYPE);
  ul_dyn_append(&generator.context.types, I8_TYPE);
  ul_dyn_append(&generator.context.types, U16_TYPE);
  ul_dyn_append(&generator.context.types, I16_TYPE);
  ul_dyn_append(&generator.context.types, U32_TYPE);
  ul_dyn_append(&generator.context.types, I32_TYPE);
  ul_dyn_append(&generator.context.types, U64_TYPE);
  ul_dyn_append(&generator.context.types, I64_TYPE);
  ul_dyn_append(&generator.context.types, VOID_TYPE);
  ul_dyn_append(&generator.context.types, CSTR_TYPE);
  ul_dyn_append(&generator.context.types, ARR_TYPE);
  generator.target = f;
  generator.failed = false;
}

type_t get_type_of_expr(ast_t expr);

type_t get_type_by_name(const char *name, bool *found) {
  if (found != NULL)
    *found = false;
  for (size_t i = 0; i < ul_dyn_length(generator.context.types); i++) {
    type_t t = dyn_type_get(generator.context.types, i);
    t.list_n = 0;
    if (streq(t.name, name)) {
      if (found != NULL)
        *found = true;
      return t;
    }
  }
  ul_assert(false, "COULD NOT FIND TYPE BY NAME");

  return (type_t){0};
}

bool is_int_type(char *name) {
  if (streq(name, "i8"))
    return true;
  if (streq(name, "u8"))
    return true;
  if (streq(name, "i16"))
    return true;
  if (streq(name, "u16"))
    return true;
  if (streq(name, "i32"))
    return true;
  if (streq(name, "u32"))
    return true;
  if (streq(name, "i64"))
    return true;
  if (streq(name, "u64"))
    return true;
  if (streq(name, "char"))
    return true;
  if (streq(name, "bool"))
    return true;
  return false;
}

type_t get_type_of_member(char *type, char *member) {
  type_t t = get_type_by_name(type, NULL);
  if (!is_int_type(type)) {

    for (size_t i = 0; i < ul_dyn_length(t.members_names); i++) {
      char *n = dyn_str_get(t.members_names, i);
      if (streq(n, member)) {
        return get_type_by_name(dyn_str_get(t.members_types, i), NULL);
      }
    }
  }
  printf("%s.%s\n", type, member);
  ul_assert(false, "COULD NOT FIND TYPE OF MEMBER");
  return (type_t){0};
}

type_t get_method_ret_type(char *name, char *method) {
  type_t t = get_type_by_name(name, NULL);
  char actual_name[128] = {0};
  // strcat(actual_name, FUN_PREFIX);
  strcat(actual_name, "__internal_");
  strcat(actual_name, name);
  strcat(actual_name, "_");
  strcat(actual_name, method);
  for (size_t i = 0; i < ul_dyn_length(t.methods); i++) {
    ast_t a = dyn_ast_get(t.methods, i);
    ast_fundef_t f = *(a->as.fundef);

    if (streq(actual_name, f.name)) {
      type_t ret = get_type_by_name(f.return_type->as.type->name, NULL);
      return ret;
    }
  }
  printf("%s\n", actual_name);
  ul_assert(false, "COULD NOT FIND RETURN TYPE OF METHOD");
  return (type_t){0};
}

type_t get_type_of_var(const char *name, bool *found_name, bool *found_type) {
  type_t res;
  for (int i = ul_dyn_length(generator.context.vars) - 1; i >= 0; i--) {
    var_t v = dyn_var_get(generator.context.vars, i);
    if (streq(v.name, name)) {
      *found_name = true;
      res = get_type_by_name(v.type, found_type);
      res.list_n = v.list_n;
      if (v.list_n > 0) {
        res.kind = TY_ARRAY;
      }
      return res;
    }
  }
  ul_assert(false, "COULD NOT FIND TYPE BY VAR");
  return (type_t){0};
}

void destroy_generator(void) {
  if (generator.target == NULL)
    return;
  fclose(generator.target);
  generator.target = NULL;
  ul_dyn_destroy(generator.context.types);
}

bool has_failed(void) { return generator.failed; }

void generate_forward(ast_t prog);
void generate_methods(ast_t prog);
void generate_epilogue();

bool type_has_constructor(char *name);
void generate_program(ast_t prog) {
  program = prog;
  ul_logger_info("Generating Program");
  generate_prolog();
  generate_forward(prog);
  ast_array_t contents = prog->as.prog->prog;
  for (size_t i = 0; i < ul_dyn_length(contents); i++) {
    ast_t stmt = dyn_ast_get(contents, i);
    generate_statement(stmt);
  }
  generate_methods(prog);
  generate_epilogue();
}

void generate_type(ast_t type) {
  bool found = false;
  char *name = type->as.type->name;
  type_t t = get_type_by_name(name, &found);
  (void)t;
  if (!found) {
    char msg[128];
    sprintf(msg, "Type %s has not been defined...", name);
    ul_logger_erro_location(type->loc, msg);
    generator.failed = true;
  }
  if (type->kind != A_IDEN) {
    if (type->as.type->list_n > 0) {
      gprintf("__internal_array_t");
    } else {
      gprintf("%s", type->as.iden->content);
    }
  } else {
    gprintf("%s", type->as.iden->content);
  }
}

void generate_fundef_param(ast_t fundef_param) {
  ul_logger_info("Generating Fundef param");

  ast_fundef_param_t f = *fundef_param->as.fundef_param;
  generate_type(f.type);
  gprintf(" %s", f.name);
  var_array_t *vs = &generator.context.vars;
  var_t var;
  strcpy(var.name, f.name);
  strcpy(var.type, f.type->as.type->name);
  var.list_n = f.type->as.type->list_n;
  ul_dyn_append(vs, var);
}

void generate_fundef(ast_t fundef) {
  ul_logger_info("Generating Fundef");
  ast_fundef_t f = *fundef->as.fundef;
  generate_type(f.return_type);
  gprintf(" %s%s(", FUN_PREFIX, f.name);
  for (size_t i = 0; i < ul_dyn_length(f.params); ++i) {
    if (i > 0) {
      gprintf(", ");
    }
    ast_t param = dyn_ast_get(f.params, i);
    generate_fundef_param(param);
  }
  gprintf("){\n");
  for (size_t i = 0; i < ul_dyn_length(f.body); ++i) {
    ast_t stmt = dyn_ast_get(f.body, i);
    generate_statement(stmt);
    gprintf("\n");
  }
  gprintf("}\n");
}

void generate_funcall(ast_t funcall) {
  ul_logger_info("Generating Funcall");

  ast_funcall_t f = *funcall->as.funcall;
  gprintf("%s%s(", FUN_PREFIX, f.name);
  for (size_t i = 0; i < ul_dyn_length(f.args); ++i) {
    if (i > 0) {
      gprintf(", ");
    }
    ast_t arg = dyn_ast_get(f.args, i);
    generate_expression(arg);
  }
  gprintf(")");
}

void generate_vardef(ast_t vardef) {
  ul_logger_info("Generating Vardef");
  ast_vardef_t v = *vardef->as.vardef;
  bool is_array = false;
  if (v.type->kind == A_IDEN) {
    is_array = false;
  } else {
    if (v.type->as.type->list_n > 0) {
      is_array = true;
    } else {
      is_array = false;
    }
  }
  if (v.value != NULL) {
    generate_type(v.type);
    gprintf(" %s", v.name);
    gprintf("=");
    generate_expression(v.value);
    gprintf(";");
    var_array_t *vs = &generator.context.vars;
    var_t var;
    strcpy(var.name, v.name);
    strcpy(var.type, v.type->as.iden->content);
    var.list_n = 0;
    ul_dyn_append(vs, var);
  } else {
    if (is_array) {
      var_array_t *vs = &generator.context.vars;
      var_t var;
      strcpy(var.name, v.name);
      strcpy(var.type, v.type->as.iden->content);
      var.list_n = v.type->as.type->list_n;
      ul_dyn_append(vs, var);
      generate_type(v.type);
      gprintf(" %s = __internal_new_array(", v.name);
      if (v.type->as.type->list_n > 1) {
        gprintf("__internal_array_t");
      } else {
        gprintf("%s, %s", v.type->as.type->name,
                is_int_type(v.type->as.type->name) ? "false" : "true");
      }
      gprintf(");");
    } else {
      bool found;
      type_t t = get_type_by_name(v.type->as.iden->content, &found);
      ul_assert_location(vardef->loc, found, "Type problem");
      var_array_t *vs = &generator.context.vars;
      var_t var;
      var.list_n = 0;
      strcpy(var.name, v.name);
      // var.type = f.type->as.iden->content;
      strcpy(var.type, v.type->as.iden->content);
      ul_dyn_append(vs, var);
      if (!t.is_builtin && t.kind == TY_STRUCT) {
        generate_type(v.type);
        gprintf(" %s;", v.name);
        gprintf("{");
        gprintf("unsigned int old_arena=get_arena();");
        gprintf("set_arena(new_arena(sizeof(struct __ul_internal_%s)));",
                t.name);
        gprintf("%s", v.name);
        gprintf("= alloc(sizeof(struct __ul_internal_%s), 1);", t.name);
        gprintf("*%s=(struct __ul_internal_%s){0};", v.name, t.name);
        gprintf("set_arena(old_arena);");
        if (type_has_constructor(t.name)) {
          gprintf(FUN_PREFIX "__internal_%s_%s(%s);", t.name, t.name, v.name);
        }
        gprintf("}");
      }
    }
  }
}

void generate_operator(token_kind_t op) {
  switch (op) {
  case T_MULT:
    gprintf("*");
    break;
  case T_DIV:
    gprintf("/");
    break;
  case T_MODULO:
    gprintf("%%");
    break;
  case T_MINUS:
    gprintf("-");
    break;
  case T_PLUS:
    gprintf("+");
    break;
  case T_GRTR:
    gprintf(">");
    break;
  case T_GRTR_EQ:
    gprintf(">=");
    break;
  case T_LSSR:
    gprintf("<");
    break;
  case T_LSSR_EQ:
    gprintf("<=");
    break;
  case T_EQ:
    gprintf("==");
    break;
  case T_DIFF:
    gprintf("!=");
    break;
  case T_AND:
    gprintf("&&");
    break;
  case T_OR:
    gprintf("||");
    break;
  case T_LOG_OR:
    gprintf("|");
    break;
  case T_LOG_AND:
    gprintf("&");
    break;
  case T_NOT:
    gprintf("!");
    break;
  default:
    ul_assert(false, "THIS KIND OF OPERATOR IS NOT IMPLEMENTED !");
  }
}

void generate_expression_as_string(ast_t stmt) {
  type_t t = get_type_of_expr(stmt);
  char *tname = t.name;
  if (streq(tname, "string"))
    generate_expression(stmt);
  else {
    if (streq(tname, "char")) {
      gprintf("__UL_char_to_string(");
      generate_expression(stmt);
      gprintf(")");
    } else if (is_int_type(tname)) {
      gprintf("__UL_string_of_%s_int(", t.is_signed ? "signed" : "unsigned");
      generate_expression(stmt);
      gprintf(")");
    } else {
      gprintf("__UL_string_of_%s(", tname);
      generate_expression(stmt);
      gprintf(")");
    }
  }
}

void generate_binop(ast_t binop) {
  ast_binop_t b = *binop->as.binop;
  type_t tl = get_type_of_expr(b.left);
  type_t tr = get_type_of_expr(b.right);
  if ((streq(tl.name, "string") || streq(tr.name, "string")) && b.op == T_EQ) {
    gprintf("__UL_streq(");
    generate_expression_as_string(b.left);
    gprintf(",");
    generate_expression_as_string(b.right);
    gprintf(")");
  } else if ((streq(tl.name, "string") || streq(tr.name, "string")) &&
             b.op == T_DIFF) {
    gprintf("!__UL_streq(");
    generate_expression_as_string(b.left);
    gprintf(",");
    generate_expression_as_string(b.right);
    gprintf(")");
  } else if ((streq(tl.name, "string") || streq(tr.name, "string")) &&
             b.op == T_PLUS) {
    gprintf("__UL_addstr(");
    generate_expression_as_string(b.left);
    gprintf(",");
    generate_expression_as_string(b.right);
    gprintf(")");
  } else {
    gprintf("(");
    generate_expression(b.left);
    generate_operator(b.op);
    generate_expression(b.right);
    gprintf(")");
  }
}

void generate_iden(ast_t iden) {
  ast_iden_t i = *iden->as.iden;
  gprintf("%s", i.content);
}

void generate_numlit(ast_t numlit) {
  ast_num_lit_t n = *numlit->as.numlit;
  gprintf("%s", n.content);
}

void generate_if(ast_t ifstmt) {
  ast_if_t i = *ifstmt->as.ifstmt;
  gprintf("if("); //
  generate_expression(i.condition);
  gprintf(")");
  generate_statement(i.ifstmt);
  if (i.elsestmt != NULL) {
    gprintf("else ");
    generate_statement(i.elsestmt);
  }
}

type_t get_ret_type_of_funcall(ast_t expr) {
  //
  ast_funcall_t f = *expr->as.funcall;
  ast_prog_t p = *program->as.prog;
  for (size_t i = 0; i < p.prog.length; i++) {
    ast_t stmt = dyn_ast_get(p.prog, i);
    if (stmt->kind == A_FUNDEF) {
      char *fname = stmt->as.fundef->name;
      if (streq(fname, f.name)) {
        ast_type_t ty = *stmt->as.fundef->return_type->as.type;
        type_t t = get_type_by_name(ty.name, NULL);
        if (ty.list_n > 0) {
          t.list_n = ty.list_n;
          t.is_builtin = false;
          t.kind = TY_ARRAY;
        }
        return t;
      }
    }
  }
  ul_assert_location(expr->loc, false, "Could not get return type of funcall");
  return (type_t){0};
}

type_t get_type_of_expr(ast_t expr) {
  switch (expr->kind) {
  case A_CHARLIT:
    return CHAR_TYPE;
  case A_STRLIT:
    return get_type_by_name("string", NULL);
  case A_NUMLIT:
    return I64_TYPE;
  case A_IDEN: {
    bool found_name;
    bool found_type;
    return get_type_of_var(expr->as.iden->content, &found_name, &found_type);
  }
  case A_ACCESS: {
    // TODO: Big refactor lol
    ast_access_t a = *expr->as.access;
    type_t t = get_type_of_expr(a.object);
    if (a.field->kind == A_FUNCALL) {
      char *n = a.field->as.funcall->name;
      if (t.kind == TY_ARRAY && (streq(n, "set"))) {
        return VOID_TYPE;
      } else if (t.kind == TY_ARRAY && (streq(n, "append"))) {
        return VOID_TYPE;
      } else if (t.kind == TY_ARRAY && (streq(n, "length"))) {
        return U32_TYPE;
      } else {
        return get_method_ret_type(
            t.kind == TY_ARRAY ? "__internal_array_t" : t.name, n);
      }
    } else {
      return get_type_of_member(t.name, a.field->as.iden->content);
    }
    break;
  }
  case A_BINOP: {
    return get_type_of_expr(expr->as.binop->left);
  } break;
  case A_INDEX: {
    type_t t = get_type_of_expr(expr->as.index->value);
    if (t.list_n > 0) {
      t.list_n -= 1;
      return t;
    }
    if (streq(t.name, "cstr") || streq(t.name, "string")) {
      return get_type_by_name("char", NULL);
    } else {
      ul_assert_location(expr->loc, false,
                         "Cannot index other types than \'cstr\' and "
                         "\'string\' for the moment.");
    }
  } break;
  case A_FUNCALL: {
    return get_ret_type_of_funcall(expr);
  }
  default:
    ul_assert_location(expr->loc, false,
                       "Cannot get type of expression yet !\n");
    return (type_t){0};
  }
  return (type_t){0};
}

void generate_access(ast_t access) {
  ast_access_t a = *access->as.access;
  type_t t = get_type_of_expr(a.object);
  // printf("FOUND ARRAY ? %d !\n", t.list_n);
  if (is_int_type(t.name) && t.list_n == 0) {
    ul_assert_location(access->loc, false, "INT TYPE IN ACCESS");
  }
  if (a.field->kind == A_IDEN) {
    generate_expression(a.object);
    gprintf("->%s", a.field->as.iden->content);
  } else {
    ast_funcall_t f = *a.field->as.funcall;
    gprintf(FUN_PREFIX "__internal_%s_%s(",
            t.list_n > 0 || t.kind == TY_ARRAY ? "__internal_array_t" : t.name,
            f.name);
    size_t i;
    for (i = 0; i < ul_dyn_length(f.args); ++i) {
      if (i > 0) {
        gprintf(", ");
      }
      ast_t arg = dyn_ast_get(f.args, i);
      generate_expression(arg);
    }
    if (i > 0) {
      gprintf(", ");
    }
    generate_expression(a.object);
    gprintf(")");
  }
}

void generate_index(ast_t index) {
  ast_index_t i = *index->as.index;
  type_t t = get_type_of_expr(i.value);
  if (streq(t.name, "string") && t.list_n == 0) {
    gprintf("(");
    generate_expression(i.value);
    gprintf(")->contents[");
    generate_expression(i.index);
    gprintf("]");
  } else if (streq(t.name, "cstr") && t.list_n == 0) {
    generate_expression(i.value);
    gprintf("[");
    generate_expression(i.index);
    gprintf("]");
  } else if (t.list_n > 0) {
    gprintf(FUN_PREFIX "__internal___internal_array_t_get(");
    generate_expression(i.index);
    gprintf(", ");
    generate_expression(i.value);
    gprintf(", %s", t.list_n > 1 ? "__internal_array_t" : t.name);
    gprintf(")");
  }
}

void generate_unary(ast_t unary) {
  ul_logger_info("Generating Unary operation");
  ast_unary_t u = *unary->as.unary;
  if (u.is_postfix) {
    gprintf("(");
    generate_expression(u.operand);
    gprintf(")");
    generate_operator(u.op);
  } else {
    generate_operator(u.op);
    gprintf("(");
    generate_expression(u.operand);
    gprintf(")");
  }
}

void generate_expression(ast_t stmt) {
  ul_logger_info("Generating Expression");
  switch (stmt->kind) {
  case A_STRLIT: {
    gprintf("__internal_cstr_to_string(%s)", stmt->as.strlit->content);
    return;
  }
  case A_FUNCALL: {
    generate_funcall(stmt);
    return;
  }
  case A_BINOP: {
    generate_binop(stmt);
    return;
  }
  case A_IDEN: {
    generate_iden(stmt);
    return;
  }
  case A_CHARLIT: {
    gprintf("%s", stmt->as.charlit->content);
    return;
  }
  case A_NUMLIT: {
    generate_numlit(stmt);
    return;
  }
  case A_ACCESS: {
    generate_access(stmt);
    return;
  }
  case A_INDEX: {
    // ul_assert(false, "yes");
    generate_index(stmt);
    return;
  }
  case A_UNARY: {
    generate_unary(stmt);
    return;
  }
  default:
    break;
  }
  ul_logger_erro(ast_kind_to_str(stmt->kind));
  ul_assert_location(stmt->loc, false,
                     "This kind of expression is not implemented yet");
}

unsigned int loops_n = 0;

void generate_loop(ast_t loop) {
  loops_n++;
  ast_loop_t l = *loop->as.loop;

  var_t var;
  strcpy(var.name, l.varname);
  strcpy(var.type, "i32");
  var.list_n = 0;
  ul_dyn_append(&generator.context.vars, var);

  gprintf("{");

  gprintf("i32 __ul_internal_init%d = ", loops_n);
  generate_expression(l.init);
  gprintf(";");

  gprintf("i32 __ul_internal_end%d = ", loops_n);
  generate_expression(l.end);
  gprintf(";");

  gprintf("i32 __ul_internal_incr%d = __ul_internal_end%d >= "
          "__ul_internal_init%d ? 1 : -1;",
          loops_n, loops_n, loops_n);

  gprintf("for(i32 %s = __ul_internal_init%d; __ul_internal_incr%d * %s %s "
          "__ul_internal_incr%d * __ul_internal_end%d; "
          "%s+=__ul_internal_incr%d )",
          l.varname, loops_n, loops_n, l.varname,
          l.strict ? "<" : "<=", loops_n, loops_n, l.varname, loops_n);

  generate_statement(l.stmt);
  gprintf("}");

  loops_n--;
}

void generate_compound(ast_t compound) {
  ast_compound_t c = *compound->as.compound;
  gprintf("{\n");
  for (size_t i = 0; i < ul_dyn_length(c.stmts); ++i) {
    generate_statement(dyn_ast_get(c.stmts, i));
  }
  gprintf("}\n");
}

void generate_return(ast_t ret) {
  ast_return_t r = *ret->as.retstmt;
  gprintf("return ");
  generate_expression(r.expr);
  gprintf(";");
}

void generate_tdef(ast_t tdef) {
  ast_tdef_t t = *tdef->as.tdef;
  // gprintf("typedef struct __ul_internal%s %s")
  gprintf("typedef struct __ul_internal_%s {", t.type.name);
  size_t i;
  for (i = 0; i < ul_dyn_length(t.type.members_names); i++) {
    char *name = dyn_str_get(t.type.members_names, i);
    char *type = dyn_str_get(t.type.members_types, i);
    gprintf("%s %s;", type, name);
  }
  if (i == 0) {
    gprintf("char __internal_dummy;");
  }
  gprintf("} __ul_internal_%s;", t.type.name);
}

bool type_has_constructor(char *name) {
  bool found;
  type_t t = get_type_by_name(name, &found);
  char constructor[128] = {0};
  strcat(constructor, "__internal_");
  strcat(constructor, name);
  strcat(constructor, "_");
  strcat(constructor, name);
  if (!found)
    return false;
  for (size_t i = 0; i < ul_dyn_length(t.methods); i++) {
    ast_t meth = dyn_ast_get(t.methods, i);
    ast_fundef_t m = *meth->as.fundef;
    if (streq(m.name, constructor)) {
      return true;
    }
  }
  return false;
}

void generate_assign(ast_t assign) {
  ast_assign_t a = *assign->as.assign;
  generate_expression(a.expr);
  gprintf("=");
  generate_expression(a.value);
  gprintf(";");
}

void generate_while(ast_t stmt) {
  ast_while_t w = *stmt->as.whilestmt;
  gprintf("while(");
  generate_expression(w.condition);
  gprintf(")");
  generate_statement(w.stmt);
}
int iter_index = 0;

void generate_iter(ast_t iter) {
  ast_iter_t i = *iter->as.iter;
  gprintf("{__internal_array_t __internal_arr%d = ", iter_index);
  generate_expression(i.itered);
  gprintf(";");
  gprintf("for(size_t __internal_index%d=0; __internal_index%d< " FUN_PREFIX
          "__internal___internal_array_t_length(__internal_arr%d); "
          "__internal_index%d++){",
          iter_index, iter_index, iter_index, iter_index);
  type_t t = get_type_of_expr(i.itered);
  if (t.list_n > 1) {
    gprintf("__internal_array_t ");
  } else {
    gprintf("%s ", t.name);
  }
  gprintf("%s = " FUN_PREFIX
          "__internal___internal_array_t_get(__internal_index%d, "
          "__internal_arr%d, ",
          i.var->as.iden->content, iter_index, iter_index);
  if (t.list_n > 1) {
    gprintf("__internal_array_t ");
  } else {
    gprintf("%s ", t.name);
  }
  gprintf(");");
  var_t v;

  strcpy(v.name, i.var->as.iden->content);
  strcpy(v.type, t.name);
  v.list_n = t.list_n - 1;
  ul_dyn_append(&generator.context.vars, v);

  iter_index++;
  generate_statement(i.stmt);
  gprintf("}}");
  iter_index--;
}

void generate_statement(ast_t stmt) {
  size_t l = ul_dyn_length(generator.context.vars);
  ul_logger_info("Generating Statement");
  bool found = true;
  switch (stmt->kind) {
  case A_FUNDEF: {
    generate_fundef(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }
  } break;
  case A_VARDEF: {
    generate_vardef(stmt);
  } break;
  case A_IF: {
    generate_if(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }

  } break;
  case A_COMPOUND: {
    generate_compound(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }
  } break;
  case A_RETURN: {
    generate_return(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }

  } break;
  case A_LOOP: {
    generate_loop(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }
  } break;
  case A_TDEF: {
    generate_tdef(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }

  } break;
  case A_ASSIGN: {
    generate_assign(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }

    // ul_assert(false, "");
  } break;
  case A_WHILE: {
    generate_while(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }

  } break;
  case A_ITER: {
    generate_iter(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }
  } break;
  default:
    found = false;
    break;
  }
  if (!found) {
    generate_expression(stmt);
    gprintf(";\n");
  }
}

void generate_fundef_prototype(ast_t stmt) {
  if (stmt->kind == A_FUNDEF) {
    ast_fundef_t f = *stmt->as.fundef;
    if (streq(f.name, "entry"))
      return;
    generate_type(f.return_type);
    gprintf(" %s%s(", FUN_PREFIX, f.name);
    for (size_t k = 0; k < ul_dyn_length(f.params); ++k) {
      if (k > 0) {
        gprintf(", ");
      }
      ast_t param = dyn_ast_get(f.params, k);
      generate_fundef_param(param);
    }
    gprintf(");");
  }
}

void generate_forward(ast_t prog) {
  ul_logger_info("Generating Forward definitions");
  ast_array_t contents = prog->as.prog->prog;
  for (size_t i = 0; i < ul_dyn_length(contents); i++) {
    ast_t stmt = dyn_ast_get(contents, i);
    if (stmt->kind == A_TDEF) {
      ast_tdef_t t = *stmt->as.tdef;
      ul_dyn_append(&generator.context.types, t.type);
      gprintf("typedef struct __ul_internal_%s * %s;", t.type.name,
              t.type.name);
      for (size_t m = 0; m < ul_dyn_length(t.type.methods); m++) {
        ast_t fdef = dyn_ast_get(t.type.methods, m);
        generate_fundef_prototype(fdef);
      }
    }
  }

  for (size_t i = 0; i < ul_dyn_length(contents); i++) {
    ast_t stmt = dyn_ast_get(contents, i);
    generate_fundef_prototype(stmt);
  }
}

void generate_methods(ast_t prog) {
  size_t tmp = ul_dyn_length(generator.context.vars);

  ul_logger_info("Generating methods definitions");
  ast_array_t contents = prog->as.prog->prog;
  for (size_t i = 0; i < ul_dyn_length(contents); i++) {
    ast_t stmt = dyn_ast_get(contents, i);
    if (stmt->kind == A_TDEF) {
      ast_tdef_t t = *stmt->as.tdef;
      for (size_t m = 0; m < ul_dyn_length(t.type.methods); m++) {
        size_t l = ul_dyn_length(generator.context.vars);
        ast_t fdef = dyn_ast_get(t.type.methods, m);
        generate_fundef(fdef);
        while (ul_dyn_length(generator.context.vars) > l) {
          ul_dyn_destroy_last(&generator.context.vars);
        }
      }
    }
  }
  while (ul_dyn_length(generator.context.vars) > tmp) {
    ul_dyn_destroy_last(&generator.context.vars);
  }
}

void generate_prolog() {
  ul_logger_info("Generating Prolog");
  char *buff;
  FILE *f = fopen("src/template/prologue.c", "r");
  fseek(f, 0, SEEK_END);
  size_t length = ftell(f);
  fclose(f);
  unsigned int old_arena = get_arena();
  set_arena(new_arena(length + 1));
  read_file("src/template/prologue.c", &buff, &length);
  set_arena(old_arena);
  gprintf("%s", buff);
}

void generate_epilogue() {
  ul_logger_info("Generating Epilogue");
  char *buff;
  FILE *f = fopen("src/template/epilogue.c", "r");
  fseek(f, 0, SEEK_END);
  size_t length = ftell(f);
  fclose(f);
  unsigned int old_arena = get_arena();
  set_arena(new_arena(length + 1));
  read_file("src/template/epilogue.c", &buff, &length);
  set_arena(old_arena);
  gprintf("%s", buff);
}
