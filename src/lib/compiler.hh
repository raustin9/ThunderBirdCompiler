#ifndef COMPILER
#define COMPILER
#pragma once

#include <string>

#include "token.hh"
#include "lexer.hh"
#include "parser.hh"
#include "symboltable.hh"
#include "errorhandler.hh"

class Compiler {
  public:
    // Member Variables
    std::string input; // this will eventually change to list of files or whatever module system is
    SymbolTable *symbol_table;
    ErrorHandler *error_handler;

    Lexer *lexer;
    Parser *parser;
  
    // Member Functions
    Compiler(std::string source_code);
    ~Compiler();

    void test_lexer();
    void test_parser(); 
};

#endif /* COMPILER */
