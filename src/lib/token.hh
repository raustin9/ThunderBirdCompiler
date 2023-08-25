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
  TOK_ILLEGAL,        // illegal
  TOK_EOF,        // end of file
  
  TOK_IDENT,      // identifiers
  TOK_INT,        // "10" or "243"
  TOK_TYPEINT,    // "int" actual integer type
  TOK_FLOAT,      // "3.14" or "200.5"
  TOK_TYPEFLOAT,  // "float" actual float type
  TOK_BYTE,       // "byte" a 1 byte integer
  TOK_TYPEBYTE,   // "byte" type specifier
  TOK_TYPEBOOL,   // "bool" type specifier
  TOK_STRING,     // "string" actual string type
  TOK_VOID,       // "void"

  TOK_TRUE,       // "true" keyword
  TOK_FALSE,      // "false" keyword
  TOK_IF,         // "if" keyword
  TOK_ELSE,       // "else" keyword
  TOK_WHILE,      // "while" keyword
  TOK_FOR,        // "for" keyword

  TOK_EQUALS,     // "="
  TOK_PLUS,       // "+"
  TOK_MINUS,      // "-"
  TOK_BANG,       // "!"
  TOK_ASTERISK,   // "*"
  TOK_SLASH,      // "/"
  TOK_MOD,        // "%"
  TOK_PLUS_EQUAL,  // "+="
  TOK_MINUS_EQUAL, // "-="
  TOK_TIMES_EQUAL, // "*="
  TOK_DIV_EQUAL,   // "/="
  TOK_MOD_EQUAL,   // "%="

  TOK_GT,         // "<"
  TOK_LT,         // ">"
  TOK_EQUALTO,    // "=="
  TOK_NOTEQUALTO, // "!="
  TOK_GTEQUALTO,  // ">="
  TOK_LTEQUALTO,  // "<="
  
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
  TOK_ENTRY,      // "entry 
};

// Enumeration of the supported data types 
enum DataType {
  TYPE_BOOL,   // boolean type
  TYPE_INT,    // integer type
  TYPE_FLOAT,  // floating point type
  TYPE_BYTE,   // byte type -- similar to 'char' from C
  TYPE_STRING, // string type -- currently unsupported -- also might eventually be changed to byte[]
  TYPE_VOID,   // void type -- currently only used by the compiler
};

// Structure for tokens that the lexer creates
typedef struct Token {
  int line_num;        // the line number of the source code the token exists in
  TokenType type;      // the actual token enum
  std::string literal; // the literal of the token -- identifier string or the symbol that it represents
} token_t;


#endif /* TOKEN_ */
