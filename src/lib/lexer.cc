#include "lexer.hh"
#include "token.hh"

// Constructor
Lexer::Lexer(std::string input) {
  this->input = input;
  this->read_char();
}

// get the next token
token_t
Lexer::next_token() {
  token_t tok;

  switch (this->cur_char) {
    case '=':
      tok.type = TOK_EQUALS;
      tok.literal = std::string(1, this->cur_char);
      break;
    case ';':
      tok.type = TOK_SEMICOLON;
      tok.literal = std::string(1, this->cur_char);
      break;
    case '(':
      tok.type = TOK_LPAREN;
      tok.literal = std::string(1, this->cur_char);
      break;
    case ')':
      tok.type = TOK_RPAREN;
      tok.literal = std::string(1, this->cur_char);
      break;
    case '{':
      tok.type = TOK_LBRACE;
      tok.literal = std::string(1, this->cur_char);
      break;
    case '}':
      tok.type = TOK_RBRACE;
      tok.literal = std::string(1, this->cur_char);
      break;
    case '[':
      tok.type = TOK_LBRACKET;
      tok.literal = std::string(1, this->cur_char);
      break;
    case ']':
      tok.type = TOK_RBRACKET;
      tok.literal = std::string(1, this->cur_char);
      break;
    case ',':
      tok.type = TOK_COMMA;
      tok.literal = std::string(1, this->cur_char);
      break;
    case '+':
      tok.type = TOK_PLUS;
      tok.literal = std::string(1, this->cur_char);
      break;
    case 0:
      tok.type = TOK_EOF;
      tok.literal = "";
      break;
  };

  this->read_char();
  return tok;
}

// read character from the input in the lexer
void
Lexer::read_char() {
  if ((size_t)this->read_position >= this->input.length()) {
    this->cur_char = 0;
  } else {
    this->cur_char = this->input[this->read_position];
  }

  this->position = this->read_position;
  this->read_position++;
  return;
}
