#include "preprocessor.hh"
#include <cctype>
#include <string>

Preprocessor::Preprocessor(std::string input) {
  this->read_position = 0;
  this->position = 0;
  this->input = input;

  this->directives["$alia"] = DIR_ALIAS;

  this->advance_char();
  this->advance_char();
}

// Go through the entire source file and transform it where necessary
// 
// COMMENTS: "//" character to the end of the line
std::string
Preprocessor::process() {
  while (this->cur_char != '\0') {
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
      case '$':
        // DIRECTIVES
        if (this->read_directive() == DIR_ALIAS) {
          printf("GOT ALIAS\n");
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
    this->cur_char = '\0';
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
    return '\0';
  } else {
    return this->input[this->read_position];
  }
}

// Read a single line comment
// replace each character with a ' ' until the end of the line
void
Preprocessor::single_line_comment() {
  while (this->cur_char != '\n') {
    printf("replacing [%c]\n", this->input[position]);
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
        printf("expected: [/]. Replacing [%c]\n", this->input[this->position]);
        this->input[this->position] = ' ';
        this->advance_char();
        printf("expected: [/]. Replacing [%c]\n", this->input[this->position]);
        this->input[this->position] = ' ';
        this->advance_char();
        break;
      }
    }
    printf("Replacing [%c]\n", this->input[this->position]);
    this->input[this->position] = ' ';
    this->advance_char();
  }
}

directive_e
Preprocessor::read_directive() {
  printf("read_directive: replacing [%c]\n", this->input[position]);
  this->input[position] = ' ';
  this->advance_char(); // eat the '$'
  std::string dir_string = "$";
  while (isalpha(this->cur_char) != 0) {
    dir_string.push_back(this->cur_char);
    printf("curchar: [%c] -- read_directive: replacing [%c]\n", this->cur_char, this->input[position]);
    this->input[position] = ' ';
    this->advance_char();
  }

  printf("FINAL DIRECTIVE: %s\n", dir_string.c_str());
  if (this->check_directive(dir_string)) {
    // valid directive
    return this->directives[dir_string];
  } else {
    // invalid directive
    return DIR_INVALID;
  }
}

bool
Preprocessor::check_directive(std::string directive) {
  if (this->directives.find(directive) != this->directives.end()) {
    return true;
  } else {
    return false;
  }
}
