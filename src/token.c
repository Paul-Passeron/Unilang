#include "../include/token.h"
#include "../include/logger.h"

void print_token(token_t tok) {
  char buffer[128] = {0};
  sprintf(buffer, "Lexeme: %s", tok.lexeme);
  ul_logger_info_location(tok.location, buffer);
}

const char *token_kind_to_str(token_kind_t t) {
  switch (t) {
  case T_STRLIT:
    return "T_STRLIT";
  case T_NUMLIT:
    return "T_NUMLIT";
  case T_CHARLIT:
    return "T_CHARLIT";
  case T_WORD:
    return "T_WORD";
  case T_PLUS:
    return "T_PLUS";
  case T_MINUS:
    return "T_MINUS";
  case T_MULT:
    return "T_MULT";
  case T_DIV:
    return "T_DIV";
  case T_MODULO:
    return "T_MODULO";
  case T_GRTR:
    return "T_GRTR";
  case T_GRTR_EQ:
    return "T_GRTR_EQ";
  case T_LSSR:
    return "T_LSSR";
  case T_LSSR_EQ:
    return "T_LSSR_EQ";
  case T_AND:
    return "T_AND";
  case T_OR:
    return "T_OR";
  case T_NOT:
    return "T_NOT";
  case T_EQ:
    return "T_EQ";
  case T_DIFF:
    return "T_DIFF";
  case T_BIGARR:
    return "T_BIGARR";
  case T_SMALLARR:
    return "T_SMALLARR";
  case T_OPENPAREN:
    return "T_OPENPAREN";
  case T_CLOSEPAREN:
    return "T_CLOSEPAREN";
  case T_OPENBRACKET:
    return "T_OPENBRACKET";
  case T_OPENBRACE:
    return "T_OPENBRACE";
  case T_CLOSEBRACKET:
    return "T_CLOSEBRACKET";
  case T_CLOSEBRACE:
    return "T_CLOSEBRACE";
  case T_COLON:
    return "T_COLON";
  case T_SEMICOLON:
    return "T_SEMICOLON";
  case T_DOT:
    return "T_DOT";
  case T_COMMA:
    return "T_COMMA";
  default:
    return "";
  }
}

int get_precedence(token_kind_t t) {
  switch (t) {
  case T_MULT:
  case T_DIV:
  case T_MODULO:
    return 11;
  case T_MINUS:
    return 10;
  case T_PLUS:
    return 9;
  case T_GRTR:
  case T_GRTR_EQ:
  case T_LSSR:
  case T_LSSR_EQ:
    return 7;
  case T_EQ:
  case T_DIFF:
    return 6;
  case T_AND:
    return 2;
  case T_OR:
    return 1;
  default:
    return -1;
  }
}