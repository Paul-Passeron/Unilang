#include "../include/ul_flow.h"
#include "../include/generator.h"
#include "../include/lexer.h"
#include "../include/logger.h"
#include "../include/parser.h"
#include "../include/ul_allocator.h"
#include "../include/ul_compiler_globals.h"
#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>

str_array_t included_files;

unsigned int inc_arena;

void ul_start(int argc, char **argv) {

  logger_severity_t severity = SEV_WARN;
  bool sev_set = false;

  char *output;
  bool output_set = false;

  char *input;
  bool input_set = false;

  create_logger(&ul_global_logger);

  for (int i = 1; i < argc; i++) {
    char *buff = argv[i];
    if (*buff != '-' && !input_set) {
      input = buff;
      input_set = true;
    } else if ((streq(buff, "-s") || streq(buff, "--silent")) && !sev_set) {
      severity = SEV_SILENT;
      sev_set = true;
    } else if ((streq(buff, "-v") || streq(buff, "--verbose")) && !sev_set) {
      severity = SEV_INFO;
      sev_set = true;
    } else if (streq(buff, "--ignore-warnings") && !sev_set) {
      severity = SEV_ERRO;
      sev_set = true;
    } else if ((streq(buff, "-o") || streq(buff, "--output")) && !output_set) {
      buff = argv[++i];
      output = buff;
      output_set = true;
    }
  }

  set_logger_severity(&ul_global_logger, severity);

  if (!input_set) {
    ul_logger_erro("You must specify an input file");
    ul_exit(1);
  }

  char default_out[] = "a.out";
  if (!output_set) {
    output = default_out;
  }

  ul_logger_info("Started Unilang compiler");
  unsigned int arena = new_arena(2048);
  set_arena(arena);

  included_files = new_str_dyn();
  inc_arena = new_arena(1024 * PATH_MAX);

  ul_logger_info("Starting Lexer");
  lexer_t l;
  new_lexer(&l, input);
  lex_program(&l);

  ul_logger_info("Starting Parser");
  parser_t p = new_parser(l.toks);
  ast_t prog = parse_program(&p);
  ul_logger_info("File successfully parsed");

  ul_logger_info("Starting Generator");
  char out[128] = {0};
  strcpy(out, output);
  strcat(out, ".c");
  set_generator_target(out);
  generate_program(prog);
  ul_logger_info("File successfully generated");
  destroy_generator();

  char command[256];
  char info[270] = {0};

  ul_logger_info("Formatting transpiled C code");
  sprintf(command, "clang-format -i %s", out);
  system(command);
  sprintf(info, "[CMD] %s", command);
  ul_logger_info(info);

  sprintf(command, "sed -i \'/^$/d\' %s", out);
  system(command);
  sprintf(info, "[CMD] %s", command);
  ul_logger_info(info);

  ul_logger_info("Compiling transpiled C code with gcc");

  sprintf(command, "/usr/bin/gcc -o %s %s", output, out);
  system(command);
  sprintf(info, "[CMD] %s", command);
  ul_logger_info(info);

  destroy_lexer(l);
  destroy_parser(p);
  destroy_arena(arena);
}

void ul_exit(unsigned char exit_code) {
  destroy_generator();
  clear_allocator();
  ul_destroy_logger();
  exit(exit_code);
}

void ul_end(void) {
  ul_logger_info("Ended Unilang compiler");
  ul_exit(0);
}