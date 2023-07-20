#include "parser.hh"
#include "token.hh"
#include "lexer.hh"

#include <string>

// Constructor
Parser::Parser(std::string input) {
  this->lex = new Lexer(input);

  // Initialize the token values
  this->current_token.type = TOK_ILLEGAL;
  this->current_token.literal = "";

  this->peek_token.type = TOK_ILLEGAL;
  this->peek_token.literal = "";

  this->next_token();
  this->next_token();
}

void
Parser::next_token() {
  this->current_token = this->peek_token;
  this->peek_token = this->lex->next_token();
}

Program*
Parser::parse_program() {
  return nullptr;
}
