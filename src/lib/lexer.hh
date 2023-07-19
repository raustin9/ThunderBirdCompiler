#pragma once
#ifndef LEXER_
#define LEXER_

#include <iostream>
#include <string>
#include <map>

#include "token.hh"

class Lexer {
  private:

  public:
    std::string input; // the source code input
    int position;      // the current position in the input
    int read_position; // the current reading position in input (after current char)
    char cur_char;     // current char under examination
    std::map<std::string, TokenType> keywords;

    Lexer(std::string input);
    token_t next_token();
    void read_char();
    std::string read_identifier();
    token_t read_number();
    char peek_char();
    TokenType lookup_identifier(std::string ident);
    void skip_whitespace();
};

#endif /* LEXER_ */
