#include "errorhandler.hh"
#include "symboltable.hh"
#include "token.hh"
#include "lexer.hh"
#include "parser.hh"
#include "compiler.hh"

// Constructor for the compiler
Compiler::Compiler(std::string input_text) {
  this->input = input_text;
  this->symbol_table = new SymbolTable();
  this->error_handler = new ErrorHandler();
  this->lexer = new Lexer(this->input);

  // Point the lexer's symbol table and error handler to the compiler's
  this->lexer->error_handler = this->error_handler;
  this->lexer->symbol_table = this->symbol_table;
}

// Destructor for the compiler
Compiler::~Compiler() {
  delete this->parser;
  delete this->lexer;
  delete this->symbol_table;
  delete this->error_handler;
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
  this->parser->error_handler = this->error_handler;
  this->parser->symbol_table = this->symbol_table;
  this->parser->parse_program();

  this->error_handler->print_errors();
  this->symbol_table->print_elements();
}
