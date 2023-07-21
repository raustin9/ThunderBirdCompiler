#include "parser.hh"
#include "ast.hh"
#include "token.hh"
#include "lexer.hh"

#include <string>
#include <cstdlib>
#include <memory>

// Constructor
Parser::Parser(std::string input) {
  this->lex = new Lexer(input);

  // Initialize the token values
  this->current_token.type = TOK_ILLEGAL;
  this->current_token.literal = "";

  this->peek_token.type = TOK_ILLEGAL;
  this->peek_token.literal = "";

  // fill in the first two tokens
  this->next_token();
  this->next_token();
}

void
Parser::next_token() {
  this->current_token = this->peek_token;
  this->peek_token = this->lex->next_token();
}

// Parse let statemetns
// "let int x = 5;"
std::unique_ptr<Statement>
Parser::parse_let_statement() {
  this->next_token(); // eat the TOK_LET token
  token_t tok = this->current_token;
  
  switch(tok.type) {
    case TOK_TYPEINT:
      printf("matched int\n");
      this->parse_assignment(TYPE_INT);
      break;
    default:
      printf("error2 unexpected token |%s|\n", tok.literal.c_str());
      break;
  }
  


  return nullptr;
}

std::unique_ptr<Expression>
Parser::parse_assignment(DataType data_type) {
  this->next_token(); // eat the Type Identifier
  token_t tok = this->current_token;

  std::string var_name = tok.literal;
  if (tok.type != TOK_IDENT) {
    printf("error: unexpected token |%s|\n", tok.literal.c_str());
    return nullptr;
  }
  printf("matched identifier\n");
  
  this->next_token(); // eat the variable
  tok = this->current_token;

  if (tok.type != TOK_EQUALS) {
    printf("error: unexpected token |%s|\n", tok.literal.c_str());
    return nullptr;
  }

  this->next_token(); // eat the TOK_EQUALS
  switch (this->current_token.type) {
    case TOK_INT:
      printf("matched int\n");
      this->parse_integer();
      break;

    default:
      printf("unexpected token: |%s|\n", this->current_token.literal.c_str());
      return nullptr;
  }

//  if (tok.type != TOK_INT) {
//    printf("error: unexpected token |%s|\n", tok.literal.c_str());
//    return nullptr;
//  } else {
//    printf("matched integer\n");
//  }

  return nullptr;
}

std::unique_ptr<Expression>
Parser::parse_integer() {
  token_t tok = this->current_token;
  if (tok.type != TOK_INT) {
    printf("error: unexpected token |%s|\n", tok.literal.c_str());
    return nullptr;
  }

  long long val = atoi(tok.literal.c_str());
  printf("matched int: val = %lld\n", val);

  return nullptr;
}

std::unique_ptr<Program>
Parser::parse_program() {
  std::unique_ptr<Program> program;

  // main loop
  while(true) {
    switch(this->current_token.type) {
      case TOK_LET:
        printf("matched let\n");
        this->parse_let_statement();
        goto exit_loop;
        break;
      default:
        printf("did not match let\n");
        break;
    }
  }
  exit_loop:

  return program;
}
