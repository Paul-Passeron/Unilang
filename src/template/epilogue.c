
string __internal_cstr_to_string(const char *contents) {
  size_t l = strlen(contents);
  unsigned int old_arena = get_arena();
  unsigned int arena = new_arena(sizeof(__ul_internal_string) + l + 1);
  set_arena(arena);
  string res = alloc(sizeof(__ul_internal_string), 1);
  char *str = alloc(l + 1, 1);
  memcpy(str, contents, l);
  str[l] = 0;
  set_arena(old_arena);
  *res = (__ul_internal_string){.contents = str, .length = l, .arena = arena};
  return res;
}

char *__UL_string_to_cstr(string s) { return s->contents; }

string __UL_new_string(u32 count) {
  unsigned int old_arena = get_arena();
  unsigned int arena = new_arena(sizeof(__ul_internal_string) + count + 1);
  set_arena(arena);
  string res = alloc(sizeof(__ul_internal_string), 1);
  res->arena = arena;
  res->contents = alloc_zero(count + 1, 1);
  res->length = 0;
  set_arena(old_arena);
  return res;
}

string __UL_char_to_string(char c) {
  string s = __UL_new_string(1);
  s->contents[0] = c;
  s->length = 1;
  return s;
}

string __UL_append_string(string dest, string to_append) {
  string s = __UL_new_string(dest->length + to_append->length + 1);
  unsigned int old_arena = dest->arena;
  for (size_t i = 0; i < dest->length; i++) {
    s->contents[i] = dest->contents[i];
    s->length++;
  }
  for (size_t i = 0; i < to_append->length; i++) {
    s->contents[i + dest->length] = to_append->contents[i];
    s->length++;
  }
  *dest = *s;
  // destroy_arena(old_arena);
  return dest;
}
