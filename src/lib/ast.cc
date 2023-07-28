#include "ast.hh"
#include "token.hh"

#include <string>
#include <cstdio>

void
Program::print() {
  for (unsigned i = 0; i < this->statements.size(); i++) {
    if (this->statements[i]) this->statements[i]->print();
  }
}

std::string
get_data_type(DataType dt) {
  switch (dt) {
    case TYPE_INT:
      return "int";
    case TYPE_FLOAT:
      return "float";
    default:
      return "invalid";
  }
}

void
ExpressionStatement::print() {
  if (!this->expr) {
    printf("null expr\n");
  } else {
    this->expr->print();
  }
}

void
Prototype::print() {
  printf("function %s %s (", get_data_type(this->ret_type).c_str(), this->name.c_str());
  for (size_t i = 0; i < this->args.size(); i++) {
    printf("%s %s", get_data_type(this->args[i].data_type).c_str(), this->args[i].name.c_str());
    if (i < this->args.size()-1)
      printf(", ");
  }
  printf(")\n");
}

void
IntegerExpr::print() {
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

  printf("intexpr val: %lld type: %s\n", this->value, dt.c_str());
}

void
IdentifierExpr::print() {
  printf("%s", this->name.c_str());
}

void
FloatExpr::print() {
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

  printf("floatexpr val: %lf type: %s\n", this->value, dt.c_str());
}

void
VariableExpr::print() {
  std::string dt;
  switch (this->data_type) {
    case TYPE_INT:
      dt = "int";
      printf("[name: '%s' type: '%s' val: %lld] ",
          this->name.c_str(), dt.c_str(), this->value);
      break;
    case TYPE_FLOAT:
      dt = "float";
      printf("[name: '%s' type: '%s' val: %lf] ",
          this->name.c_str(), dt.c_str(), this->dvalue);
      break;
    case TYPE_STRING:
      dt = "string";
      break;
    default:
      dt = "invalid";
      break;
  }

}

void
VariableAssignment::print() {
  this->variable->print();
  printf(" %s ", this->op.literal.c_str());
  if (this->RHS)
    this->RHS->print();
}

void
BinaryExpr::print() {
  // printf("lhs: ");
  printf("\n");
  this->LHS->print();
  
  printf(" %s ", this->op.literal.c_str());
  
  // printf("rhs: ");
  this->RHS->print();
  printf("\n");
}

void
Statement::print() {}

void
Expression::print() {
  
}

void
LetStmt::print() {
  printf("%s ", this->token.literal.c_str());
  if (this->var_assign)
    this->var_assign->print();
  else 
    printf("invalid variable assignment\n");
}
