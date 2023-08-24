#include "preprocessor.hh"
#include <string>

Preprocessor::Preprocessor(std::string input) {
  this->read_position = 0;
  this->position = 0;
  this->input = input;

  this->advance_char();
}

// Go through the entire source file and transform it where necessary
// 
// COMMENTS: "//" character to the end of the line
std::string
Preprocessor::process() {
  while (this->cur_char != EOF) {
    switch (this->cur_char) {
      case '/':
        if (this->peek_char() == '/') {
          // single line comment
          printf("SINGLE LINE COMMENT\n");
          this->single_line_comment();
        } else if (this->peek_char() == '*') {
          // multi-line comment
          printf("MULTI_LINE_COMMENT\n");
          this->multi_line_comment();
        }
        break;
      default:
        break;
    }

    this->advance_char();
  }

  return this->input;
}

void
Preprocessor::advance_char() {
  if (this->read_position >= this->input.length()) {
    this->cur_char = EOF;
  } else {
    this->cur_char = this->input[this->read_position];
  }

  this->position = this->read_position;
  this->read_position++;
  return;
}

char
Preprocessor::peek_char() {
  if ((size_t)this->read_position >= this->input.length()) {
    return EOF;
  } else {
    return this->input[this->read_position];
  }
}

// Read a single line comment
// replace each character with a ' ' until the end of the line
void
Preprocessor::single_line_comment() {
  while (this->cur_char != '\n') {
    this->input[this->position] = ' ';
    this->advance_char();
  }
}

// Read a multi-line comment
// replace each character with a ' ' until "*/" is read
void
Preprocessor::multi_line_comment() {
  while (1) {
    if (this->cur_char == '*') {
      if (this->peek_char() == '/') {
        // read "*/" -- end of comment
        printf("expected: [/]. Replacing [%c]\n", this->input[this->read_position]);
        this->input[this->position] = ' ';
        this->advance_char();
        break;
      }
    }
    this->input[this->position] = ' ';
    this->advance_char();
  }
}
