#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include "lexer.hh"
#include "token.hh"
#include "ast.hh"
#include "parser.hh"

bool test_lexer();
bool test_let();
std::string read_file(char *file_name);


int
main(int argc, char** argv) {
  std::string file_name;

  if (argc < 2) {
//    if (test_lexer() == true) {
//      printf("-- all tests passed --\n");
//    }
    if (test_let() == true) {
      printf("let statement passed\n");
    }
  } else {
    // Read the file specified through the command line argument
    // ignore all other args for now
    file_name = argv[1];
    printf("fdn: %s\n", file_name.c_str());
    std::string input = read_file(argv[1]);
    std::cout << input << std::endl;

    Lexer *lex = new Lexer(input);
    for (token_t tok; tok.type != TOK_EOF && tok.type != TOK_ILLEGAL; tok = lex->next_token()) 
      std::cout << tok.literal << std::endl;
  }

  return 0;
}

bool
test_let() {
//  std::string input = 
//    "if (x < 2) {\n"
//    "  let int x = y + z * z;\n"
//    "  if (x > 4) {\n"
//    "    let int z = 5;\n"
//    "  }\n"
//    "}\n"
//    ;
  std::string input =
    "define int main() {\n"
    "  if (x > 4) {\n"
    "    let int x = 5;\n"
    "  }\n"
    "  return x;\n"
    "}\n";

  printf("input:\n%s\n\n", input.c_str());
  Parser *parser = new Parser(input);

  parser->parse_program();

  delete parser;
  return true;
}


// read input from file
std::string
read_file(char *file_name) {
  std::string rv;
  FILE* fp;
  char *file_content;
  size_t file_size;
  struct stat st;

  if (access(file_name, F_OK) != 0) {
    perror("read_file (access)");
    return "";
  }

  fp = fopen(file_name, "r");
  if (fp == NULL) {
    fclose(fp);
    perror("read_file (fopen)");
    return "";
  }

  // Get size of the file and read it all in
  stat(file_name, &st);
  file_size = st.st_size;

  file_content = (char*)calloc(file_size, sizeof(char));
  if (fread(file_content, file_size, 1, fp) != 1) {
    fclose(fp);
    perror("read_file (fread)");
    free(file_content);
    return "";
  }

  // file read successfully
  fclose(fp);
  rv = file_content;
  free(file_content);
  return rv;
}

bool
test_lexer() {
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
      return false;
    }
  }

  return true;
}
