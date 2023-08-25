#include "errorhandler.hh"
#include "preprocessor.hh"
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

  this->preprocessor = new Preprocessor(this->input);
  printf("----\n%s\n-----\n", this->preprocessor->process().c_str());
  this->lexer = new Lexer(this->preprocessor->process());
  // this->lexer = new Lexer(this->input);

  // Point the lexer's symbol table and error handler to the compiler's
  this->lexer->error_handler = this->error_handler;
}

// Destructor for the compiler
Compiler::~Compiler() {
  delete this->parser;
  delete this->lexer;
  delete this->symbol_table;
  delete this->error_handler;
  delete this->preprocessor;
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
  std::shared_ptr<AST> ast = this->parser->create_ast();

  this->error_handler->print_errors();
}
