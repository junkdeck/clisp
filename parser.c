#include <stdio.h>
#include <stdlib.h>

#include <editline/history.h>
#include <editline/readline.h>

#include "lib/mpc/mpc.h"

long eval_op(long x, char *op, long y) {
  if (strcmp(op, "+") == 0) {
    return x + y;
  }
  if (strcmp(op, "-") == 0) {
    return x - y;
  }
  if (strcmp(op, "*") == 0) {
    return x * y;
  }
  if (strcmp(op, "/") == 0) {
    return x / y;
  }
  return 0;
}

int eval(mpc_ast_t *t) {
  if (strstr(t->tag, "number")) {
    // current tag is number
    return atoi(t->contents);
  }

  // operator is always second child
  char *op = t->children[1]->contents;

  long x = eval(t->children[2]);

  // iterate remaining children and combine
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}

int main(int argc, char **argv) {

  // Create parsers
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Clisp = mpc_new("clisp");

  // Define grammar
  mpca_lang(MPCA_LANG_DEFAULT,
            "                                                 \
      number  : /-?[0-9]+[.]?[0-9]*/ ;                  \
      operator: '+' | '-' | '/' | '*' | '%'             \
      | /add/ | /sub/ | /mul/ | /mod/ | /div/ ;         \
      expr    : <number> | '(' <operator> <expr>+ ')';  \
      clisp   : /^/ <operator> <expr>+ /$/ ;            \
      ",
            Number, Operator, Expr, Clisp);

  puts("Clisp v0.0.1");
  puts("Ctrl-c to exit\n");

  while (1) {
    // outputs prompt
    char *input = readline("clisp> ");

    add_history(input);

    // store result of parsing into r
    mpc_result_t r;

    // pass memory address of result to mpc_parse
    if (mpc_parse("<stdin>", input, Clisp, &r)) {
      // if successfully parsed, print the output of result
      // delete value afterwards to avoid leftovers

      long e = eval(r.output);

      printf("> %ld\n", e);

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
