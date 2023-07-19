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
    "let ten<float> = 1.0.0;\n"
    "function <int> add (x<int>, y<int>) {\n"
    "  return x + y;\n"
    "}\n"
    "";
  std::vector<TokenType> expected3 = {
    TOK_LET, TOK_IDENT, TOK_LT, TOK_TYPEFLOAT, TOK_GT, TOK_EQUALS, TOK_ILLEGAL, TOK_SEMICOLON,
    TOK_FUNCTION, TOK_LT, TOK_TYPEINT, TOK_GT, TOK_IDENT, TOK_LPAREN, TOK_IDENT, TOK_LT, TOK_TYPEINT, TOK_GT, TOK_COMMA, TOK_IDENT, TOK_LT, TOK_TYPEINT, TOK_GT, TOK_RPAREN, TOK_LBRACE,
    TOK_RETURN, TOK_IDENT, TOK_PLUS, TOK_IDENT, TOK_SEMICOLON,
    TOK_RBRACE,
    TOK_EOF
  };
  std::vector<std::string> expected3lits = {
    "let", "ten", "<", "float", ">", "=", "1.0.0", ";",
    "function", "<", "int", ">", "add", "(", "x", "<", "int", ">", ",", "y", "<", "int", ">", ")", "{",
    "return", "x", "+", "y", ";",
    "}",
    ""
  };


  std::string input4 = 
    "if (5 <= 4) {\n"
    "  return true;\n"
    "}\n"
    "else {\n"
    "  return false;\n"
    "}\n"
    "!-/*5;\n"
    "";

  std::vector<TokenType> expected4 = {
    TOK_IF, TOK_LPAREN, TOK_INT, TOK_LTEQUALTO, TOK_INT, TOK_RPAREN, TOK_LBRACE,
    TOK_RETURN, TOK_TRUE, TOK_SEMICOLON,
    TOK_RBRACE,
    TOK_ELSE,TOK_LBRACE,
    TOK_RETURN, TOK_FALSE, TOK_SEMICOLON,
    TOK_RBRACE,
    TOK_BANG, TOK_MINUS, TOK_SLASH, TOK_ASTERISK, TOK_INT, TOK_SEMICOLON,
    TOK_EOF
  };

  std::vector<std::string> expected4lits = {
    "if", "(", "5", "<=", "4", ")", "{",
    "return", "true", ";",
    "}",
    "else", "{",
    "return", "false", ";",
      "}",
    "!", "-", "/", "*", "5", ";",
    ""
  };

  std::vector<TokenType> expected = expected4;
  std::vector<std::string> expectedlits = expected4lits;

  std::string input = input4;

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
