#include "ast.hh"
#include "token.hh"

#include <string>
#include <cstdio>

void
Program::print() {}

void
IntegerExpr::print() {
  printf("intexpr val: %lld\n", this->value);
}

void
VariableExpr::print() {
  std::string dt;
  switch (this->data_type) {
    case TYPE_INT:
      dt = "int";
      break;
    case TYPE_FLOAT:
      dt = "float";
      break;
    case TYPE_STRING:
      dt = "string";
      break;
    default:
      dt = "invalid";
      break;
  }

  printf("variable expr:\n\tname: %s\n\ttype: %s\n\tval: %lld\n",
      this->name.c_str(), dt.c_str(), this->value);
}

void
VariableAssignment::print() {
  this->variable->print();
  printf(" %s \n", this->op.literal.c_str());
  this->RHS->print();
}

void
BinaryExpr::print() {}

void
Statement::print() {}

void
Expression::print() {}

void
LetStmt::print() {
  printf("%s\n", this->token.literal.c_str());
  this->var_assign->print();
}
