// GENERATOR SOURCE FILE
// Paul Passeron

#include "../include/generator.h"
#include "../include/logger.h"
#include "../include/ul_allocator.h"
#include "../include/ul_assert.h"
#include "../include/ul_compiler_globals.h"
#include "../include/ul_flow.h"
#include "../include/ul_io.h"

generator_t generator;

#define FUN_PREFIX "__UL_"
#define gprintf(...) fprintf(generator.target, __VA_ARGS__)

// Create builtin types

const type_t U8_TYPE = {.name = "u8",
                        .is_builtin = true,
                        .size = 1,
                        .kind = TY_PRIMITIVE,
                        .is_signed = false};

const type_t I8_TYPE = {.name = "i8",
                        .is_builtin = true,
                        .size = 1,
                        .kind = TY_PRIMITIVE,
                        .is_signed = true};

const type_t CHAR_TYPE = {.name = "char",
                          .is_builtin = true,
                          .size = 1,
                          .kind = TY_PRIMITIVE,
                          .is_signed = true};

const type_t U16_TYPE = {.name = "u16",
                         .is_builtin = true,
                         .size = 2,
                         .kind = TY_PRIMITIVE,
                         .is_signed = false};

const type_t I16_TYPE = {.name = "i16",
                         .is_builtin = true,
                         .size = 2,
                         .kind = TY_PRIMITIVE,
                         .is_signed = true};

const type_t U32_TYPE = {.name = "u32",
                         .is_builtin = true,
                         .size = 4,
                         .kind = TY_PRIMITIVE,
                         .is_signed = false};

const type_t I32_TYPE = {.name = "i32",
                         .is_builtin = true,
                         .size = 4,
                         .kind = TY_PRIMITIVE,
                         .is_signed = true};

const type_t U64_TYPE = {.name = "u64",
                         .is_builtin = true,
                         .size = 8,
                         .kind = TY_PRIMITIVE,
                         .is_signed = false};

const type_t I64_TYPE = {.name = "i64",
                         .is_builtin = true,
                         .size = 8,
                         .kind = TY_PRIMITIVE,
                         .is_signed = true};

const type_t VOID_TYPE = {.name = "void",
                          .is_builtin = true,
                          .size = 0,
                          .kind = TY_PRIMITIVE,
                          .is_signed = false};

const type_t STRING_TYPE = {.name = "string",
                            .is_builtin = true,
                            .size = 24,
                            .kind = TY_PRIMITIVE,
                            .is_signed = false};

const type_t CSTR_TYPE = {.name = "cstr",
                          .is_builtin = true,
                          .size = 8,
                          .kind = TY_PRIMITIVE,
                          .is_signed = false};

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
  ul_dyn_append(&generator.context.types, I8_TYPE);
  ul_dyn_append(&generator.context.types, U16_TYPE);
  ul_dyn_append(&generator.context.types, I16_TYPE);
  ul_dyn_append(&generator.context.types, U32_TYPE);
  ul_dyn_append(&generator.context.types, I32_TYPE);
  ul_dyn_append(&generator.context.types, U64_TYPE);
  ul_dyn_append(&generator.context.types, I64_TYPE);
  ul_dyn_append(&generator.context.types, VOID_TYPE);
  ul_dyn_append(&generator.context.types, STRING_TYPE);
  ul_dyn_append(&generator.context.types, CSTR_TYPE);
  generator.target = f;
  generator.failed = false;
}

type_t get_type_by_name(const char *name, bool *found) {
  *found = false;
  for (size_t i = 0; i < ul_dyn_length(generator.context.types); i++) {
    type_t t = dyn_type_get(generator.context.types, i);
    if (streq(t.name, name)) {
      *found = true;
      return t;
    }
  }
  return (type_t){0};
}

type_t get_type_of_var(const char *name, bool *found_name, bool *found_type) {
  for (size_t i = 0; i < ul_dyn_length(generator.context.vars); i++) {
    var_t v = dyn_var_get(generator.context.vars, i);
    if (streq(v.name, name)) {
      *found_name = true;
      return get_type_by_name(v.type, found_type);
    }
  }

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

void generate_program(ast_t prog) {
  ul_logger_info("Generating Program");
  generate_prolog();
  generate_forward(prog);
  ast_array_t contents = prog->as.prog->prog;
  for (size_t i = 0; i < ul_dyn_length(contents); i++) {
    ast_t stmt = dyn_ast_get(contents, i);
    generate_statement(stmt);
  }
}

void generate_type(ast_t type) {
  bool found = false;
  char *name = type->as.iden->content;
  type_t t = get_type_by_name(name, &found);
  (void)t;
  if (!found) {
    char msg[128];
    sprintf(msg, "Type %s has not been defined...", name);
    ul_logger_erro_location(type->loc, msg);
    generator.failed = true;
  }
  if (type->kind == A_IDEN) {
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
  // var.name = f.name;
  strcpy(var.name, f.name);
  if (f.type->kind == A_IDEN) {
    // var.type = f.type->as.iden->content;
    strcpy(var.type, f.type->as.iden->content);
  } else {
    ul_assert(false, "WTF");
  }
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
  // type_array_t ts = generator.context.types;
  ast_vardef_t v = *vardef->as.vardef;

  if (v.value != NULL) {

    generate_type(v.type);
    gprintf(" %s", v.name);
    gprintf("=");
    generate_expression(v.value);
    gprintf(";");
    var_array_t *vs = &generator.context.vars;
    var_t var;
    strcpy(var.name, v.name);
    // var.type = f.type->as.iden->content;
    strcpy(var.type, v.type->as.iden->content);
    ul_dyn_append(vs, var);
  } else {
    bool found;
    type_t t = get_type_by_name(v.type->as.iden->content, &found);
    ul_assert_location(vardef->loc, found, "Type problem");
    if (!t.is_builtin && t.kind == TY_STRUCT) {
      generate_type(v.type);
      gprintf(" %s;", v.name);
      gprintf("{");
      gprintf("unsigned int old_arena=get_arena();");
      gprintf("set_arena(new_arena(sizeof(struct __ul_internal_%s)));", t.name);
      gprintf("%s", v.name);
      gprintf("= alloc(sizeof(struct __ul_internal_%s), 1);", t.name);
      gprintf("*%s=(struct __ul_internal_%s){0};", v.name, t.name);
      gprintf("set_arena(old_arena);");
      gprintf("}");
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
  default:
    break;
  }
}

void generate_binop(ast_t binop) {
  ast_binop_t b = *binop->as.binop;
  gprintf("(");
  generate_expression(b.left);
  generate_operator(b.op);
  generate_expression(b.right);
  gprintf(")");
  //
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

type_t get_type_of_expr(ast_t expr) {
  switch (expr->kind) {
  case A_CHARLIT:
    return CHAR_TYPE;
  case A_STRLIT:
    return STRING_TYPE;
  case A_NUMLIT:
    return I64_TYPE;
  case A_IDEN: {
    bool found_name;
    bool found_type;
    if (expr->kind == A_IDEN) {
      return get_type_of_var(expr->as.iden->content, &found_name, &found_type);
    } else {
      ul_assert(false, "Cannot get type of expression yet !\n");
      return (type_t){0};
    }
  }
  default:
    ul_assert(false, "Cannot get type of expression yet !\n");
    return (type_t){0};
  }
}

void generate_access(ast_t access) {
  ast_access_t a = *access->as.access;
  // Should do some checks here
  type_t t = get_type_of_expr(a.object);

  if (a.field->kind == A_IDEN) {
    generate_expression(a.object);
    gprintf("->%s", a.field->as.iden->content);
  } else {
    if (streq(t.name, "string")) {
      ast_funcall_t f = *a.field->as.funcall;
      if (streq(f.name, "append")) {
        gprintf("__UL_append_string(");
        generate_expression(a.object);
        gprintf(",");
        for (size_t i = 0; i < ul_dyn_length(f.args); ++i) {
          if (i > 0) {
            gprintf(", ");
          }
          ast_t arg = dyn_ast_get(f.args, i);
          generate_expression(arg);
        }
        gprintf(")");
      }
    } else {
      ul_assert(false, "Metdhods are not implemented yet !");
    }
  }
}

void generate_index(ast_t index) {
  ast_index_t i = *index->as.index;
  type_t t = get_type_of_expr(i.value);
  if (streq(t.name, "string")) {
    gprintf("(");
    generate_expression(i.value);
    gprintf(").contents[");
    generate_expression(i.index);
    gprintf("]");
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
    ul_assert(false, "yes");
    generate_index(stmt);
    return;
  }
  default:
    break;
  }
  ul_logger_erro(ast_kind_to_str(stmt->kind));
  ul_assert(false, "This kind of expression is not implemented yet");
}

unsigned int loops_n = 0;

void generate_loop(ast_t loop) {
  loops_n++;
  ast_loop_t l = *loop->as.loop;
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
  for (size_t i = 0; i < ul_dyn_length(t.type.members_names); i++) {
    char *name = dyn_str_get(t.type.members_names, i);
    char *type = dyn_str_get(t.type.members_types, i);
    gprintf("%s %s;", type, name);
  }
  gprintf("} __ul_internal_%s;", t.type.name);
}

void generate_assign(ast_t assign) {
  ast_assign_t a = *assign->as.assign;
  generate_expression(a.expr);
  gprintf("=");
  generate_expression(a.value);
  gprintf(";");
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
  } break;
  case A_COMPOUND: {
    generate_compound(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }
  } break;
  case A_RETURN: {
    generate_return(stmt);
  } break;
  case A_LOOP: {
    generate_loop(stmt);
    while (ul_dyn_length(generator.context.vars) > l) {
      ul_dyn_destroy_last(&generator.context.vars);
    }
  } break;
  case A_TDEF: {
    generate_tdef(stmt);
  } break;
  case A_ASSIGN: {
    generate_assign(stmt);
    // ul_assert(false, "");
  } break;
  default:
    found = false;
    break;
  }
  if (!found) {
    generate_expression(stmt);
    gprintf(";\n");
  }
  // printf("%ld %ld\n", ul_dyn_length(generator.context.vars), l);
}
void generate_forward(ast_t prog) {
  ast_array_t contents = prog->as.prog->prog;
  for (size_t i = 0; i < ul_dyn_length(contents); i++) {
    ast_t stmt = dyn_ast_get(contents, i);
    if (stmt->kind == A_TDEF) {
      ast_tdef_t t = *stmt->as.tdef;
      ul_dyn_append(&generator.context.types, t.type);
      gprintf("typedef struct __ul_internal_%s * %s;", t.type.name,
              t.type.name);
    }
  }
  for (size_t i = 0; i < ul_dyn_length(contents); i++) {
    ast_t stmt = dyn_ast_get(contents, i);
    if (stmt->kind == A_FUNDEF) {
      ast_fundef_t f = *stmt->as.fundef;
      if (streq(f.name, "entry"))
        continue;
      generate_type(f.return_type);
      gprintf(" %s%s(", FUN_PREFIX, f.name);
      for (size_t i = 0; i < ul_dyn_length(f.params); ++i) {
        if (i > 0) {
          gprintf(", ");
        }
        ast_t param = dyn_ast_get(f.params, i);
        generate_fundef_param(param);
      }
      gprintf(");");
    }
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