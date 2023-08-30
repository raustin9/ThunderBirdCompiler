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
  std::shared_ptr<Program> program;

  std::shared_ptr<AST> create_ast(); // create the abstract syntax tree

  // Parsing functions
  std::shared_ptr<Program> parse_program();                         // parse the top level program
  std::shared_ptr<Statement> parse_code_block();                    // parse a block of code
  std::shared_ptr<Statement> parse_let_statement();                 // parse let statements
  std::shared_ptr<Statement> parse_return_statement();              // parse return statements
  std::shared_ptr<Statement> parse_else_statement();                // parse else clauses and else-if clauses
  std::shared_ptr<Statement> parse_if_statement();                  // parse if statements
  std::shared_ptr<Statement> parse_while_statement();               // parse while loop statements
  std::shared_ptr<Statement> parse_for_statement();                 // parse for loop statements
  std::shared_ptr<Statement> parse_function_defn();                 // parse function prototypes
  std::shared_ptr<Expression> parse_integer();                      // parse an integer literal
  std::shared_ptr<Expression> parse_byte();                         // parse an byte literal
  std::shared_ptr<Expression> parse_float();                        // parse floating point literal
  std::shared_ptr<Expression> parse_identifier();                   // parse an identifier expression
  std::shared_ptr<Expression> parse_boolean();                      // parse a boolean literal
  std::shared_ptr<Statement> parse_expression_statement();          // parse expression statement wrapper
  std::shared_ptr<Expression> parse_expr(int precedence, std::shared_ptr<Expression> LHS);                         // parse expressions
  std::shared_ptr<Expression> parse_parentheses_expr();             // parse expressions contained within parentheses
                                                                    
  std::shared_ptr<Expression> parse_prefix_op();                               // parse a prefix (unary) operator "!a"
  std::shared_ptr<Expression> parse_infix_op(std::unique_ptr<Expression> rhs); // parse an infix (binary) operator -- "a + b"
  std::shared_ptr<Expression>parse_primary(); // parse members of an expression
  std::shared_ptr<Expression>parse_expression_interior(); // parses expression that are not top level

  token_t current_token; // the current token that the parser is 'looking at'
  token_t peek_token;    // the next token that the parser would be looking at
  std::map <int, int> operator_precedences;
};

#endif /* PARSER_ */
