#pragma once
#ifndef LEXER_
#define LEXER_

#include <iostream>
#include <string>

#include "token.hh"

class Lexer {
  private:

  public:
    std::string input; // the source code input
    int position;      // the current position in the input
    int read_position; // the current reading position in input (after current char)
    char cur_char;     // current char under examination

    Lexer(std::string input);
    token_t next_token();
    void read_char();
};

#endif /* LEXER_ */
