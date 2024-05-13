#include "../include/lexer.h"
#include "../include/ul_allocator.h"
#include <stdlib.h>
#include <stdio.h>
#include "../include/logger.h"
void new_lexer(lexer_t *l, const char *path) { //
  l->filename = path;
  l->current_loc.col = 1;
  l->current_loc.line = 1;
  l->current_loc.filename = path;
  l->buffer_index = 0;
  l->state = LS_DEFAULT;
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    char buffer[1024] = {0};
    sprintf(buffer, "Could not load file \'%s\'", path);
    ul_logger_erro(buffer);
    exit(1);
  }
  fseek(f, 0, SEEK_END);
  size_t length = ftell(f);
  l->buffer_length = length;
  fseek(f, 0, SEEK_SET);
  l->buffer = alloc(length, 1);
  fread(l->buffer, 1, length, f);
  fclose(f);
}

char consume_char(lexer_t *l) {
  if (is_end_of_file(*l))
    return 0;
  char res = l->buffer[l->buffer_index++];
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
