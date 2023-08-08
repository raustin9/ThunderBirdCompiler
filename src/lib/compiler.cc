#include "token.hh"
#include "lexer.hh"
#include "parser.hh"
#include "compiler.hh"

// Constructor for the compiler
Compiler::Compiler(std::string input_text) {
  this->input = input_text;
  this->lexer = new Lexer(this->input);
  this->parser = new Parser(this->input);
}

// Destructor for the compiler
Compiler::~Compiler() {
  delete this->parser;
  delete this->lexer;

}
