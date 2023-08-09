#pragma once
#ifndef LEXER_
#define LEXER_

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "token.hh"
#include "symboltable.hh"
#include "errorhandler.hh"

class Lexer {
  private:

  public:
    std::string input; // the source code input
    int line_num;      // the line number that we are currently on
    int position;      // the current position in the input
    int read_position; // the current reading position in input (after current char)
    char cur_char;     // current char under examination
    std::map<std::string, TokenType> keywords;
    std::vector <token_t> tokens;
    ErrorHandler *error_handler; // a pointer to the -compilers's- error handler -- THIS IS OWNED BY THE COMPILER DO NOT FREE
    SymbolTable *symbol_table;   // a pointer to the -compiler's'- symbol table  -- THIS IS OWNED BY THE COMPILER DO NOT FREE

    Lexer(std::string input);
    Lexer(Lexer &l);
    token_t next_token();
    void read_char();
    std::string read_identifier();
    token_t read_number();
    char peek_char();
    TokenType lookup_identifier(std::string ident);
    void skip_whitespace();
    void tokenize_input(); // iterate through the entire input and create a token stream
};

#endif /* LEXER_ */
