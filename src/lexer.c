#include "../include/lexer.h"
#include "../include/logger.h"
#include "../include/ul_allocator.h"
#include "../include/ul_assert.h"
#include "../include/ul_compiler_globals.h"
#include "../include/ul_flow.h"
#include "../include/ul_io.h"
#include <libgen.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>

extern str_array_t included_files;
extern unsigned int inc_arena;

const char *lexer_state_to_str(lexer_state_t state) {
  switch (state) {
  case LS_DEFAULT:
    return "LS_DEFAULT";
  case LS_SINGLE_LINE_COMMENT:
    return "LS_SINGLE_LINE_COMMENT";
  case LS_MULTI_LINE_COMMENT:
    return "LS_MULTI_LINE_COMMENT";
  case LS_STRING_LIT:
    return "LS_STRING_LIT";
  case LS_NUM_LIT:
    return "LS_NUM_LIT";
  case LS_WORD:
    return "LS_WORD";
  case LS_DELIMITER:
    return "LS_DELIMITER";
  case LS_INCLUDE:
    return "LS_INCLUDE";
  default:
    return "";
  }
}

void new_lexer(lexer_t *l, char *path) {
  unsigned int old_arena = get_arena();

  FILE *f = fopen(path, "r");
  fseek(f, 0, SEEK_END);
  l->buffer_length = ftell(f);
  fclose(f);
  l->arena = new_arena(l->buffer_length * 2 + PATH_MAX);
  set_arena(l->arena);
  read_file(path, &l->buffer, &l->buffer_length);
  l->filename = alloc(PATH_MAX, 1);
  strcpy(l->filename, path);
  l->current_loc.col = 1;
  l->current_loc.line = 1;
  l->current_loc.filename = l->filename;
  l->buffer_index = 0;
  l->state = LS_DEFAULT;
  l->toks = new_tok_dyn();
  set_arena(old_arena);
}

void destroy_lexer(lexer_t l) { destroy_arena(l.arena); }

char consume_char(lexer_t *l) {
  if (is_end_of_file(*l))
    return 0;
  char res = l->buffer[l->buffer_index++];
  l->current_loc.col++;
  if (res == '\n') {
    l->current_loc.col = 1;
    l->current_loc.line++;
  }
  return res;
}

char peek_char(lexer_t l) {
  if (is_end_of_file(l))
    return 0;
  return l.buffer[l.buffer_index];
}

char peek_n_chars(lexer_t l, size_t n) {
  for (unsigned int i = 0; i < n - 1; i++)
    consume_char(&l);
  return peek_char(l);
}

bool is_end_of_file(lexer_t l) { return l.buffer_index >= l.buffer_length; }

bool matches_string(lexer_t l, const char *str) {
  while (*str) {
    if (consume_char(&l) != *(str++))
      return false;
  }
  return true;
}

bool is_digit(lexer_t l) {
  char c = peek_char(l);
  for (int i = 0; i < 10; ++i) {
    if (c == '0' + i)
      return true;
  }
  return false;
}

bool is_whitespace(lexer_t l) {
  char c = peek_char(l);
  return c == ' ' || c == '\n';
}

bool is_delimiter(lexer_t l) {
  char c = peek_char(l);
  char delimiters[] = ":.{}()[];,+*-|<>!=&/%";
  for (size_t i = 0; i < sizeof(delimiters) - 1; i++) {
    if (c == delimiters[i])
      return true;
  }
  return false;
}

bool is_word(lexer_t l) {
  char c = peek_char(l);
  char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
  for (size_t i = 0; i < sizeof(chars) - 1; i++) {
    if (c == chars[i])
      return true;
  }
  return false;
}

bool step_default(lexer_t *l) {

  char inc_directive[] = "@include";

  if (matches_string(*l, inc_directive)) {
    for (size_t i = 0; i < strlen(inc_directive); i++) {
      consume_char(l);
    }
    l->state = LS_INCLUDE;
    return true;
  }

  if (matches_string(*l, "/*")) {
    // We have a multi line comment
    l->state = LS_MULTI_LINE_COMMENT;
    // Consumming "/*"
    (void)consume_char(l);
    (void)consume_char(l);
    return true;
  }

  if (matches_string(*l, "//")) {
    // We have a multi line comment
    l->state = LS_SINGLE_LINE_COMMENT;
    // Consumming "/*"
    (void)consume_char(l);
    (void)consume_char(l);
    return true;
  }

  if (matches_string(*l, "\"")) {
    l->state = LS_STRING_LIT;
    // (void)consume_char(l);
    return true;
  }

  if (matches_string(*l, "\'")) {
    l->state = LS_CHAR_LIT;
    // (void)consume_char(l);
    return true;
  }

  if (is_digit(*l)) {
    l->state = LS_NUM_LIT;
    return true;
  }

  if (is_delimiter(*l)) {
    l->state = LS_DELIMITER;
    return true;
  }
  if (is_word(*l)) {
    l->state = LS_WORD;
    return true;
  } else {
    char c = peek_char(*l);
    char buffer[128];
    sprintf(buffer, "Unexpected character '%c'", c);
    ul_logger_erro_location(l->current_loc, buffer);
    ul_exit(1);
  }
  return true;
}

bool step_multi(lexer_t *l) {
  while (!matches_string(*l, "*/") && !is_end_of_file(*l))
    consume_char(l);
  // */
  consume_char(l); // *
  consume_char(l); // /
  l->state = LS_DEFAULT;
  return true;
}

bool step_single(lexer_t *l) {
  while (peek_char(*l) != '\n' && !is_end_of_file(*l)) {
    consume_char(l);
  }
  consume_char(l); // \n
  l->state = LS_DEFAULT;
  return true;
}

bool step_word(lexer_t *l) {
  char buffer[128] = {0};
  location_t loc = l->current_loc;
  size_t current_index = 0;
  while (!is_end_of_file(*l) && !is_delimiter(*l) && !is_whitespace(*l) &&
         current_index < 127) {
    buffer[current_index++] = consume_char(l);
  }
  token_t tok;
  tok.location = loc;
  tok.kind = T_WORD;
  unsigned int old_arena = get_arena();
  set_arena(l->arena);
  tok.lexeme = alloc_preset_ptr(1, current_index + 1, buffer);
  tok.lexeme[current_index] = 0;
  set_arena(old_arena);
  ul_dyn_append(&l->toks, tok);
  l->state = LS_DEFAULT;
  return true;
}

bool step_strlit(lexer_t *l) {
  char buffer[1025] = {0};
  location_t loc = l->current_loc;
  size_t current_index = 0;
  buffer[current_index++] = consume_char(l);
  while (!is_end_of_file(*l) && !matches_string(*l, "\"") &&
         current_index < 1024) {
    buffer[current_index++] = consume_char(l);
  }
  buffer[current_index++] = consume_char(l);
  token_t tok;
  tok.location = loc;
  tok.kind = T_STRLIT;
  unsigned int old_arena = get_arena();
  set_arena(l->arena);
  tok.lexeme = alloc_preset_ptr(1, current_index + 1, buffer);
  tok.lexeme[current_index] = 0;
  set_arena(old_arena);
  ul_dyn_append(&l->toks, tok);
  l->state = LS_DEFAULT;
  return true;
}

bool step_charlit(lexer_t *l) {
  char buffer[5] = {0};
  location_t loc = l->current_loc;
  size_t current_index = 0;
  buffer[current_index++] = consume_char(l);
  while (!is_end_of_file(*l) && !matches_string(*l, "\'") &&
         current_index < 1024) {
    buffer[current_index++] = consume_char(l);
  }
  buffer[current_index++] = consume_char(l);
  token_t tok;
  tok.location = loc;
  tok.kind = T_CHARLIT;
  unsigned int old_arena = get_arena();
  set_arena(l->arena);
  tok.lexeme = alloc_preset_ptr(1, current_index + 1, buffer);
  tok.lexeme[current_index] = 0;
  set_arena(old_arena);
  ul_dyn_append(&l->toks, tok);
  l->state = LS_DEFAULT;
  return true;
}

bool step_numlit(lexer_t *l) {
  location_t loc = l->current_loc;
  char buffer[65];
  size_t current_index = 0;
  bool has_point = false;
  while ((is_digit(*l) || (!has_point && peek_char(*l) == '.')) &&
         !is_end_of_file(*l) && current_index < 64) {
    if (peek_char(*l) == '.')
      has_point = true;
    buffer[current_index++] = consume_char(l);
  }

  if (peek_char(*l) == 'f' && has_point) {
    buffer[current_index++] = consume_char(l);
  }
  buffer[current_index++] = 0;

  token_t tok;
  tok.location = loc;
  tok.kind = T_NUMLIT;
  unsigned int old_arena = get_arena();
  set_arena(l->arena);
  tok.lexeme = alloc_preset_ptr(1, current_index + 1, buffer);
  tok.lexeme[current_index] = 0;
  set_arena(old_arena);
  ul_dyn_append(&l->toks, tok);
  l->state = LS_DEFAULT;
  return true;
}

#define OPCOUNT 29
static char delimiters[OPCOUNT][4] = {
    "->>", "->", "=>", "==", "!=", "&&", "||", ">=", "<=", "<",
    ">",   "+",  "-",  "/",  "*",  "%",  "!",  "(",  ")",  "[",
    "]",   "{",  "}",  ";",  ",",  ".",  ":",  "&",  "|",
};

static token_kind_t dels_kinds[] = {
    T_SMALLARRLARGE, T_SMALLARR,  T_BIGARR,     T_EQ,         T_DIFF,
    T_AND,           T_OR,        T_GRTR_EQ,    T_LSSR_EQ,    T_LSSR,
    T_GRTR,          T_PLUS,      T_MINUS,      T_DIV,        T_MULT,
    T_MODULO,        T_NOT,       T_OPENPAREN,  T_CLOSEPAREN, T_OPENBRACKET,
    T_CLOSEBRACKET,  T_OPENBRACE, T_CLOSEBRACE, T_SEMICOLON,  T_COMMA,
    T_DOT,           T_COLON,     T_LOG_AND,    T_LOG_OR};

bool step_delim(lexer_t *l) {
  location_t loc = l->current_loc;
  for (size_t i = 0; i < OPCOUNT; i++) {
    if (matches_string(*l, delimiters[i])) {
      token_t tok;
      for (size_t k = 0; k < strlen(delimiters[i]); k++) {
        (void)consume_char(l);
      }
      tok.lexeme = delimiters[i];
      tok.kind = dels_kinds[i];
      tok.location = loc;
      ul_dyn_append(&l->toks, tok);
      l->state = LS_DEFAULT;
      return true;
    }
  }
  return false;
}

bool step_include(lexer_t *l) {
  char buffer[128] = {0};
  if (!matches_string(*l, "\"")) {
    // error
    ul_exit(1);
  }
  consume_char(l);
  size_t current_index = 0;
  while (!is_end_of_file(*l) && !matches_string(*l, "\"") &&
         current_index < 1024) {
    buffer[current_index++] = consume_char(l);
  }
  consume_char(l);
  l->state = LS_DEFAULT;
  char path[256] = {0};
  char fn[256] = {0};
  strcpy(fn, l->filename);
  strcpy(path, dirname(fn));
  strcat(path, "/");
  strcat(path, buffer);
  unsigned int old_arena = get_arena();
  set_arena(inc_arena);
  char *rpath = alloc(PATH_MAX, 1);
  set_arena(old_arena);
  realpath(path, rpath);

  bool path_found = false;

  for (size_t i = 0; i < ul_dyn_length(included_files); i++) {
    char *str = dyn_str_get(included_files, i);
    if (streq(rpath, str)) {
      path_found = true;
      break;
    }
  }
  if (!path_found) {
    ul_dyn_append(&included_files, rpath);
    lexer_t included_file_l;
    new_lexer(&included_file_l, path);
    lex_program(&included_file_l);
    for (size_t i = 0; i < ul_dyn_length(included_file_l.toks); i++) {
      token_t to_append = dyn_tok_get(included_file_l.toks, i);
      ul_dyn_append(&l->toks, to_append);
    }
  }
  l->state = LS_DEFAULT;
  // l->current_loc =
  return true;
}

bool step_lexer(lexer_t *l) {
  char buffer[128];
  sprintf(buffer, "Stepping lexer with state %s", lexer_state_to_str(l->state));
  ul_logger_info_location(l->current_loc, buffer);
  if (is_end_of_file(*l))
    return false;
  while (is_whitespace(*l)) {
    (void)consume_char(l);
  }
  if (is_end_of_file(*l))
    return false;
  switch (l->state) {
  case LS_DEFAULT: {
    return step_default(l);
  } break;
  case LS_SINGLE_LINE_COMMENT: {
    return step_single(l);
  } break;
  case LS_MULTI_LINE_COMMENT: {
    return step_multi(l);
  } break;
  case LS_STRING_LIT: {
    return step_strlit(l);
  } break;
  case LS_CHAR_LIT: {
    return step_charlit(l);
  } break;
  case LS_NUM_LIT: {
    return step_numlit(l);
  } break;
  case LS_WORD: {
    return step_word(l);
  } break;
  case LS_DELIMITER: {
    return step_delim(l);
  } break;
  case LS_INCLUDE: {
    return step_include(l);
  } break;
  default: {
    ul_assert(false, "Unexpected lexer state");
    return false;
  }
  }
}

void lex_program(lexer_t *l) {
  while (step_lexer(l))
    ;
  ul_logger_info("File successfully lexed");
}