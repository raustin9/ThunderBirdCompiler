#include "lexer.hh"
#include "token.hh"
#include <cctype>

// Constructor
Lexer::Lexer(std::string input) {
  this->input = input;
  this->read_char();

  this->keywords["function"] = TOK_FUNCTION;
  this->keywords["return"] = TOK_RETURN;
  this->keywords["let"] = TOK_LET;
  this->keywords["int"] = TOK_TYPEINT;
  this->keywords["float"] = TOK_TYPEFLOAT;
  this->keywords["void"] = TOK_VOID;
  this->keywords["string"] = TOK_STRING;
}

// get the next token
token_t
Lexer::next_token() {
  token_t tok;

  this->skip_whitespace();
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
    case '<':
      // future: account for '<<' for bitshifting
      tok.type = TOK_LT;
      tok.literal = std::string(1, this->cur_char);
      break;
    case '>':
      // future: account for '>>' for bitshifting
      tok.type = TOK_GT;
      tok.literal = std::string(1, this->cur_char);
      break;
    case 0:
      tok.type = TOK_EOF;
      tok.literal = "";
      break;
    default:
      if (isalpha(this->cur_char) != 0) {
        tok.literal = this->read_identifier();
        tok.type = this->lookup_identifier(tok.literal);
        return tok; // we return early here because read_identifier() advances this->cur_char repeatedly
      } else if (isdigit(this->cur_char) != 0) {
        // for now assume all number are ints
        // tok.type = TOK_INT;
        tok = this->read_number();
        return tok;
      } else {
        tok.type = TOK_ILLEGAL;
        tok.literal = std::string(1, this->cur_char);
      }
  };

  this->read_char();
  return tok;
}

//std::string
token_t
Lexer::read_number() {
  token_t tok;
  int pos = this->position;
  bool read_decimal = false;
  bool is_legal = true;

  while (isdigit(this->cur_char) != 0 || this->cur_char == '.') {
    if (this->cur_char == '.' && read_decimal == true) {
      // 2 decimal points in one number
      is_legal = false;
    }

    if (this->cur_char == '.') read_decimal = true;
    this->read_char();
  }

  if (is_legal == false) {
    tok.type = TOK_ILLEGAL;
  } else if (read_decimal == true) {
    tok.type = TOK_FLOAT;
  } else {
    tok.type = TOK_INT;
  }

  tok.literal = this->input.substr(pos, this->position-pos);

  return tok;
}

// Read an identifier string 
std::string
Lexer::read_identifier() {
  int pos = this->position;
  while (isalpha(this->cur_char) != 0) {
    this->read_char();
  }

  return this->input.substr(pos, this->position-pos);
}

// Lookup the identifier in the map of keywords to determine
// if it is a keyword or normal identifier
TokenType
Lexer::lookup_identifier(std::string ident) {
  if (this->keywords.find(ident) != this->keywords.end()) {
    return this->keywords[ident];
  }

  return TOK_IDENT;
}

// eat whitespace until we read a non-whitespace character
void
Lexer::skip_whitespace() {
  while (
    this->cur_char == ' ' ||
    this->cur_char == '\n' ||
    this->cur_char == '\t' ||
    this->cur_char == '\r'
  ) {
    this->read_char();
  }
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
