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
 
  std::unique_ptr<Program> parse_program();
  std::unique_ptr<Statement> parse_let_statement();
  std::unique_ptr<Expression> parse_integer();
  std::unique_ptr<Expression> parse_float();
  std::unique_ptr<Expression> parse_assignment(DataType data_type); 

  Lexer *lex;
  token_t current_token;
  token_t peek_token;
};

#endif /* PARSER_ */
