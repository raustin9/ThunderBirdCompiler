#include <iostream>
#include <cstdio>
#include "lexer.hh"

int
main(void) {
  std::cout << "Hello world\n";

  Lexer *lex = new Lexer;
  lex->val = 15;

  printf("val: %d\n", lex->val);

  return 0;
}
