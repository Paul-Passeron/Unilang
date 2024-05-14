#include "../include/token.h"
#include "../include/logger.h"

void print_token(token_t tok) {
  char buffer[128] = {0};
  sprintf(buffer, "Lexeme: %s", tok.lexeme);
  ul_logger_info_location(tok.location, buffer);
}
