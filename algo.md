Parse statement:
  if token is '{' -> parse compound
  if token is 'if' -> parse if
  if token is 'return' -> parse return
  if token is 'iter' -> parse iter
  if token is 'while' -> parse while
  if token is 'loop' -> parse loop
  if is fundef -> parse fundef
  if is let -> parse vardef
  else -> parse expression + consume ';'

Parse expression:
  parse_expression_aux(-1)

parse_expression_aux(min_prec):
  left = parse leaf
  while true:
    lookahead = peek type
    if lookahead isn't an operator
      break
    op_precedence = precedence(lookahead)
    if op_prec < min_prec
      break
    consume()
    right = parse_expression_aux(op_prec)
    left = Binop(op, left, right)
  return left

parse_leaf:
  ast res = null
  if is funcall
    res = parse funcall
  if type is identifier
    res = parse identifier
  if type is strlit
    res = parse_strlit
  if type is numlit
    res = parse_numlit
  if type is charlit
    res = parse_charlit
  if res == null
    ERROR
  if current == '['
    consume()
    ast expr = parse_expression
    expect(']')
    return Indexing(value = res, index = expr)
  return res
  
...

```C++
int get_precedence_aux(token_type_t t) {
  switch (t) {
  case TOK_SUB:
    return 1;
  case TOK_STAR:
    return 2;
  case TOK_DIV:
    return 2;
  case TOK_MODULO:
    return 2;
  case TOK_PLUS:
    return 4;
  case TOK_MINUS:
    return 3;
  case TOK_GRTR:
    return 6;
  case TOK_LSSR:
    return 6;
  case TOK_LSSR_EQ:
    return 6;
  case TOK_GRTR_EQ:
    return 6;
  case TOK_EQUAL:
    return 7;
  case TOK_DIFF:
    return 7;
  case TOK_BIT_AND:
    return 8;
  case TOK_BIT_XOR:
    return 9;
  case TOK_BIT_OR:
    return 10;
  case TOK_LOG_AND:
    return 11;
  case TOK_LOG_OR:
    return 12;
  default:
    return -1;
  }
}
int get_precedence(token_type_t t) {
  // Doing this because our precedence is reversed

  int res = get_precedence_aux(t);
  if (res == -1)
    return -1;
  return 13 - res;
}```