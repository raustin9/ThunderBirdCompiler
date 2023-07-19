/*
 *  token.hh
 *
 *  This file contains data structures related to tokens of the language
 *
 */

#pragma once
#ifndef TOKEN_
#define TOKEN_

#include <string>

/* KEYWORDS */
/*
function -- declares a function
return   -- function return statement
let      -- declares a variable
int      -- 64bit integer data type
float    -- 64bit float data type
string   -- string data type
void     -- void data type (for functions that return nothing)

in the future:
class
*/

/* The tokens for ThunderBird */
enum TokenType {
  ILLEGAL,        // illegal
  TOK_EOF,        // end of file
  
  TOK_IDENT,      // identifiers
  TOK_INT,        // "int"
  TOK_FLOAT,      // "float"
  TOK_STRING,     // "string"

  TOK_EQUALS,     // "="
  TOK_PLUS,       // "+"
  TOK_MINUS,      // "-"
  TOK_BANG,       // "!"
  TOK_ASTERISK,   // "*"
  TOK_SLASH,      // "/"

  TOK_GT,         // "<"
  TOK_LT,         // ">"
  TOK_EQUALTO,    // "=="
  TOK_NOTEQUALTO, // "!="
  
  TOK_COMMA,      // ","
  TOK_SEMICOLON,  // ";"

  TOK_LPAREN,     // "("
  TOK_RPAREN,     // ")"
  TOK_LBRACE,     // "{"
  TOK_RBRACE,     // "}"
  TOK_LBRACKET,   // "["
  TOK_RBRACKET,   // "]"
          
  TOK_FUNCTION,   // "function"
  TOK_RETURN,     // "return"
  TOK_LET,        // "let"

};

typedef struct Token {
  TokenType type;
  std::string literal;
} token_t;

#endif /* TOKEN_ */
