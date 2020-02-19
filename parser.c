#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "lib/mpc/mpc.h"

int main(int argc, char** argv) {

  // Create parsers
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Clisp = mpc_new("clisp");

  // Define grammar
  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                 \
      number  : /-?[0-9]+[.]?[0-9]*/ ;                            \
      operator: '+' | '-' | '/' | '*' | '%'             \
      | /add/ | /sub/ | /mul/ | /mod/ | /div/ ;          \
      expr    : <number> | '(' <operator> <expr>+ ')';  \
      clisp   : /^/ <operator> <expr>+ /$/ ;            \
      ",
      Number, Operator, Expr, Clisp);

  puts("Clisp v0.0.1");
  puts("Ctrl-c to exit\n");

  while(1) {
    // outputs prompt
    char* input = readline("clisp> ");

    add_history(input);

    // store result of parsing into r 
    mpc_result_t r;


    // pass memory address of result to mpc_parse
    if(mpc_parse("<stdin>", input, Clisp, &r)) {
      // if successfully parsed, print the output of result
      // delete value afterwards to avoid leftovers
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    // free memory
    free(input);
  }

  mpc_cleanup(4, Number, Operator, Expr, Clisp);

  return 0;
}
