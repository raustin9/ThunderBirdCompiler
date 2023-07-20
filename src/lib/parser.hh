/*
 * parser.hh
 *
 * This file contains the structure for the parser of 
 * the ThunderBird compiler
 */

#pragma once
#ifndef PARSER_
#define PARSER_

#include "lexer.hh"
#include "ast.hh"
#include <string>

class Parser {
public:
  Parser(std::string input);
  void next_token();
  Program* parse_program();

  Lexer *lex;
  token_t current_token;
  token_t peek_token;
};

#endif /* PARSER_ */
