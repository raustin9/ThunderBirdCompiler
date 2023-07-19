#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include "lexer.hh"
#include "token.hh"

int
main(void) {
  std::string input1 = "=+(){}[],;";
  std::string input2 = 
    "let five = 5;\n"
    "let ten = 10;\n"
    "function add(x, y) {\n"
    " return x + y;\n"
    "}\n"
    "let result = add(five, ten);\n"
    ;


  std::vector<TokenType> expected1 = {
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

  std::vector<TokenType> expected2 = {
    TOK_LET, TOK_IDENT, TOK_EQUALS, TOK_INT, TOK_SEMICOLON,
    TOK_LET, TOK_IDENT, TOK_EQUALS, TOK_INT, TOK_SEMICOLON,
    TOK_FUNCTION, TOK_IDENT, TOK_LPAREN, TOK_IDENT, TOK_COMMA, TOK_IDENT, TOK_RPAREN, TOK_LBRACE,
    TOK_RETURN, TOK_IDENT, TOK_PLUS, TOK_IDENT, TOK_SEMICOLON,
    TOK_RBRACE,
    TOK_LET, TOK_IDENT, TOK_EQUALS, TOK_IDENT, TOK_LPAREN, TOK_IDENT, TOK_COMMA, TOK_IDENT, TOK_RPAREN, TOK_SEMICOLON,
    TOK_EOF
  };

  std::vector<std::string> expected2lits = {
    "let", "five", "=", "5", ";",
    "let", "ten", "=", "10", ";",
    "function", "add", "(", "x", ",", "y", ")", "{",
    "return", "x", "+", "y", ";", 
    "}",
    "let", "result", "=", "add", "(", "five", ",", "ten", ")", ";",
    ""
  };

  std::string input3 = 
    "let ten<int> = 10;\n"
    "function <int> add (x<int>, y<int>) {\n"
    "  return x + y;\n"
    "}\n"
    "";
  std::vector<TokenType> expected3 = {
    TOK_LET, TOK_IDENT, TOK_LT, TOK_TYPEINT, TOK_GT, TOK_EQUALS, TOK_INT, TOK_SEMICOLON,
    TOK_FUNCTION, TOK_LT, TOK_TYPEINT, TOK_GT, TOK_IDENT, TOK_LPAREN, TOK_IDENT, TOK_LT, TOK_TYPEINT, TOK_GT, TOK_COMMA, TOK_IDENT, TOK_LT, TOK_TYPEINT, TOK_GT, TOK_RPAREN, TOK_LBRACE,
    TOK_RETURN, TOK_IDENT, TOK_PLUS, TOK_IDENT, TOK_SEMICOLON,
    TOK_RBRACE,
    TOK_EOF
  };
  std::vector<std::string> expected3lits = {
    "let", "ten", "<", "int", ">", "=", "10", ";",
    "function", "<", "int", ">", "add", "(", "x", "<", "int", ">", ",", "y", "<", "int", ">", ")", "{",
    "return", "x", "+", "y", ";",
    "}",
    ""
  };


  std::vector<TokenType> expected = expected3;
  std::vector<std::string> expectedlits = expected3lits;

  std::string input = input3;

  Lexer *lex = new Lexer(input);

  for (unsigned i = 0; i < expected.size(); i++) {
    token_t tok = lex->next_token();

    if (tok.type == expected[i] && tok.literal == expectedlits[i]) {
      printf("matched %s = %s\n", tok.literal.c_str(), expectedlits[i].c_str());
    } else {
      printf("unexpected token |%s|. expected |%s|\n", tok.literal.c_str(), expectedlits[i].c_str());
      exit(1);
    }
  }

  printf(" -- all tests passed --\n");

  return 0;
}
