#include "token.hh"
#include "lexer.hh"
#include "parser.hh"
#include "compiler.hh"

// Constructor for the compiler
Compiler::Compiler(std::string input_text) {
  this->input = input_text;
  this->lexer = new Lexer(this->input);
}

// Destructor for the compiler
Compiler::~Compiler() {
  delete this->parser;
  delete this->lexer;

}

void
Compiler::test_lexer() {

}

// Function to test the parser
void
Compiler::test_parser() {
  this->lexer->tokenize_input();
  std::vector<token_t> tokens = this->lexer->tokens;
  this->parser = new Parser(tokens);
  this->parser->parse_program();
}
