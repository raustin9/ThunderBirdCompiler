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
#include "errorhandler.hh"
#include <string>
#include <map>

// Parser class
// Parses the token stream and generates
// the Abstract Syntax Tree
class Parser {
public:
  Parser(std::vector<token_t> token_stream);  // constructor -- uses string parameter to initialize a lexer
  ~Parser();                  // destructor -- deletes the lexer that it created

  ErrorHandler *error_handler; // error hander given to the parser by the compiler -- DO NOT FREE: THIS SHOULD BE GIVEN BACK TO THE COMPILER WHEN WE ARE DONE
  SymbolTable *symbol_table;   // symbol table given to the parser by the compiler -- DO NOT FREE: THIS SHOULD BE GIVEN BACK TO THE COMPILER WHEN WE ARE DONE
  void next_token();          // eats current token and advances the peek and current tokens
  int get_token_precedence(); // gets the precedence for the current token
  bool has_entry;             // true if there is an entry point false otherwise
  std::vector <token_t> token_stream; // token stream to parse
  size_t current_position;               // the current position we are at in the token stream

  // Parsing functions
  std::unique_ptr<Program> parse_program();                         // parse the top level program
  std::shared_ptr<Statement> parse_code_block();                    // parse a block of code
  std::unique_ptr<Statement> parse_let_statement();                 // parse let statements
  std::unique_ptr<Statement> parse_return_statement();              // parse return statements
  std::unique_ptr<Statement> parse_else_statement();                // parse else clauses and else-if clauses
  std::unique_ptr<Statement> parse_if_statement();                  // parse if statements
  std::unique_ptr<Statement> parse_while_statement();               // parse while loop statements
  std::unique_ptr<Statement> parse_for_statement();                 // parse for loop statements
  std::shared_ptr<Statement> parse_function_defn();                 // parse function prototypes
  std::unique_ptr<Expression> parse_integer();                      // parse an integer literal
  std::unique_ptr<Expression> parse_float();                        // parse floating point literal
  std::unique_ptr<Expression> parse_identifier();                   // parse an identifier expression
  std::unique_ptr<Expression> parse_boolean();                      // parse a boolean literal
  std::unique_ptr<Statement> parse_expression_statement();          // parse expression statement wrapper
  std::unique_ptr<Expression> parse_expr(int precedence, std::unique_ptr<Expression> LHS);                         // parse expressions
  std::unique_ptr<Expression> parse_parentheses_expr();             // parse expressions contained within parentheses
                                                                    
  std::unique_ptr<Expression> parse_prefix_op();                               // parse a prefix (unary) operator "!a"
  std::unique_ptr<Expression> parse_infix_op(std::unique_ptr<Expression> rhs); // parse an infix (binary) operator -- "a + b"
  std::unique_ptr<Expression>parse_primary(); // parse members of an expression
  std::unique_ptr<Expression>parse_expression_interior(); // parses expression that are not top level

  token_t current_token; // the current token that the parser is 'looking at'
  token_t peek_token;    // the next token that the parser would be looking at
  std::map <int, int> operator_precedences;
};

#endif /* PARSER_ */
