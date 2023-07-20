#include "ast.hh"
#include "token.hh"

#include <string>

std::string
Program::token_literal() {
  if (this->statements.size() > 0) {
    return this->statements[0].token_literal();
  } else {
    return "";
  }
}

// Let statement implementation
void
LetStatement::statement_node() {}

std::string
LetStatement::token_literal() {
  return this->token.literal;
}

void
Identifier::expression_node() {}

std::string
Identifier::token_literal() {
  return this->token.literal;
}
