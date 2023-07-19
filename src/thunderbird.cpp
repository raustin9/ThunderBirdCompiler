#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include "lexer.hh"
#include "token.hh"

int
main(void) {
  std::string input = "=+(){}[],;";
  Lexer *lex = new Lexer(input);

  std::vector<TokenType> expected = {
    TOK_EQUALS,
    TOK_PLUS,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_COMMA,
    TOK_SEMICOLON
  };

  for (unsigned i = 0; i < input.length(); i++) {
    token_t tok = lex->next_token();

    if (tok.type == expected[i]) {
      printf("matched %c\n", input[i]);
    } else if (tok.type != expected[i]) {
      fprintf(stderr, "unexpected token %d at %c\n", tok.type, input[i]);
      exit(1);
    }
  }

  return 0;
}
