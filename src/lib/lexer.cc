#include "lexer.hh"
#include "token.hh"
#include <cctype>

// Constructor
Lexer::Lexer(std::string input) {
  this->line_num = 1;
  this->input = input;
  this->read_char();

  this->keywords["function"] = TOK_FUNCTION;
  this->keywords["define"] = TOK_FUNCTION;
  this->keywords["return"] = TOK_RETURN;
  this->keywords["let"] = TOK_LET;
  this->keywords["henceforth"] = TOK_LET;
  this->keywords["int"] = TOK_TYPEINT;
  this->keywords["float"] = TOK_TYPEFLOAT;
  this->keywords["byte"] = TOK_TYPEBYTE;
  this->keywords["bool"] = TOK_TYPEBOOL;
  this->keywords["void"] = TOK_VOID;
  this->keywords["string"] = TOK_STRING;
  this->keywords["if"] = TOK_IF;
  this->keywords["else"] = TOK_ELSE;
  this->keywords["while"] = TOK_WHILE;
  this->keywords["for"] = TOK_FOR;
  this->keywords["true"] = TOK_TRUE;
  this->keywords["false"] = TOK_FALSE;
  this->keywords["entry"] = TOK_ENTRY;
}


// Copy constructor
Lexer::Lexer(Lexer &l) {
  input = l.input;
  keywords = l.keywords;
  position = l.position;
  read_position = l.read_position;
  cur_char = l.cur_char;
}

// Iterate over entire input and create a token stream
void
Lexer::tokenize_input() {
  token_t tok;
  while (tok.type != TOK_EOF) {
    tok = this->next_token();
  }
}

// get the next token
token_t
Lexer::next_token() {
  token_t tok;

  this->skip_whitespace();
  switch (this->cur_char) {
    case '=':
      if (this->peek_char() == '=') {
        this->read_char();
        tok.type = TOK_EQUALTO;
        tok.literal = "==";
        tok.line_num = this->line_num;
      } else {
        tok.type = TOK_EQUALS;
        tok.literal = std::string(1, this->cur_char);
        tok.line_num = this->line_num;
      }
      break;
    case ';':
      tok.type = TOK_SEMICOLON;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case '(':
      tok.type = TOK_LPAREN;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case ')':
      tok.type = TOK_RPAREN;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case '{':
      tok.type = TOK_LBRACE;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case '}':
      tok.type = TOK_RBRACE;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case '[':
      tok.type = TOK_LBRACKET;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case ']':
      tok.type = TOK_RBRACKET;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case ',':
      tok.type = TOK_COMMA;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case '+':
      tok.type = TOK_PLUS;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case '<':
      // future: account for '<<' for bitshifting
      if (this->peek_char() == '=') {
        this->read_char();
        tok.type = TOK_LTEQUALTO;
        tok.literal = "<=";
        tok.line_num = this->line_num;
      } else {
        tok.type = TOK_LT;
        tok.literal = std::string(1, this->cur_char);
        tok.line_num = this->line_num;
      }
      break;
    case '>':
      // future: account for '>>' for bitshifting
      if (this->peek_char() == '=') {
        this->read_char();
        tok.type = TOK_GTEQUALTO;
        tok.literal = ">=";
        tok.line_num = this->line_num;
      } else {
        tok.type = TOK_GT;
        tok.literal = std::string(1, this->cur_char);
        tok.line_num = this->line_num;
      }
      break;
    case '!':
      if (this->peek_char() == '=') {
        this->read_char();
        tok.type = TOK_NOTEQUALTO;
        tok.literal = "!=";
        tok.line_num = this->line_num;
      } else {
        tok.type = TOK_BANG;
        tok.literal = std::string(1, this->cur_char);
        tok.line_num = this->line_num;
      }
      break;
    case '*':
      tok.type = TOK_ASTERISK;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case '/':
      tok.type = TOK_SLASH;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case '-':
      tok.type = TOK_MINUS;
      tok.literal = std::string(1, this->cur_char);
      tok.line_num = this->line_num;
      break;
    case '\0':
      tok.type = TOK_EOF;
      tok.literal = "";
      tok.line_num = this->line_num;
      break;
    default:
      if (isalpha(this->cur_char) != 0) {
        tok.literal = this->read_identifier();
        tok.type = this->lookup_identifier(tok.literal);
        tok.line_num = this->line_num;
        this->tokens.push_back(tok);
        return tok; // we return early here because read_identifier() advances this->cur_char repeatedly
      } else if (isdigit(this->cur_char) != 0) {
        // for now assume all number are ints
        // tok.type = TOK_INT;
        tok = this->read_number();
        tok.line_num = this->line_num;
        this->tokens.push_back(tok);
        return tok;
      } else {
        tok.type = TOK_ILLEGAL;
        tok.literal = std::string(1, this->cur_char);
        tok.line_num = this->line_num;
        char err[50];
        sprintf(err, "lexer: illegal token %s", tok.literal.c_str());
        this->error_handler->new_error(tok.line_num, err);
      }
  };

  this->read_char();
  this->tokens.push_back(tok);
  return tok;
}

char
Lexer::peek_char() {
  if ((size_t)this->read_position >= this->input.length()) {
    return 0;
  } else {
    return this->input[this->read_position];
  }
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
  while (isalpha(this->cur_char) != 0 || this->cur_char == '_' || isdigit(this->cur_char) != 0) {
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
    if (this->cur_char == '\n')
      this->line_num++;
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
