#ifndef COMPILER
#define COMPILER
#pragma once

#include <string>

#include "token.hh"
#include "lexer.hh"
#include "parser.hh"
#include "symboltable.hh"

class Compiler {
  public:
    std::string input; // this will eventually change to list of files or whatever module system is
    SymbolTable symbol_table;

    Lexer *lexer;
    Parser *parser;
  
    Compiler(std::string source_code);
    ~Compiler();
};

#endif /* COMPILER */
