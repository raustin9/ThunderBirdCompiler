#include "ast.hh"
#include "token.hh"

#include <locale>
#include <string>
#include <cstdio>
#include <memory>

void
Program::print() {
  for (unsigned i = 0; i < this->statements.size(); i++) {
    if (this->statements[i]) this->statements[i]->print();
    printf("\n");
  }
}

std::string
get_data_type(DataType dt) {
  switch (dt) {
    case TYPE_INT:
      return "int";
    case TYPE_FLOAT:
      return "float";
    case TYPE_BYTE:
      return "byte";
    case TYPE_BOOL:
      return "bool";
    case TYPE_VOID:
      return "void";
    default:
      return "invalid";
  }
}

void
ReturnStmt::print() {
  printf("%s ", this->token.literal.c_str());
  this->ret_val->print();
}

void
ExpressionStatement::print() {
  if (!this->expr) {
    printf("null expr\n");
  } else {
    this->expr->print();
  }
}

// FUNCTION //
void
FunctionDecl::print() {
  if (this->is_entry)
    printf("\nentry %s %s (", get_data_type(this->prototype->ret_type).c_str(), this->prototype->name.c_str());
  else
    printf("\ndefine %s %s (", get_data_type(this->prototype->ret_type).c_str(), this->prototype->name.c_str());
  
  for (size_t i = 0; i < this->prototype->params.size(); i++) {
    printf("%s %s", get_data_type(this->prototype->params[i].data_type).c_str(), this->prototype->params[i].name.c_str());
    if (i < this->prototype->params.size()-1)
      printf(", ");
  }
  printf(") {\n\n");
  if (this->func_body)
    this->func_body->print();

  printf("\n} end [%s]\n", this->prototype->name.c_str());
//  printf("---- %s symbol table ----\n", this->prototype->name.c_str());
//  dynamic_cast<CodeBlock*>(this->func_body.get())->symbol_table->print_elements();
//  printf("------------------------\n");
}

// Create symbol table entry from fields in the FunctionDecl class
std::unique_ptr<SymbolTableEntry>
FunctionDecl::get_st_entry() {
  auto symbol_table_entry = std::make_unique<SymbolTableEntry>(
                              this->prototype->name,
                              this->prototype->ret_type,
                              64, // size -- 64 bits for both floats and ints
                              1,  // dimensions -- 1 because we do not parse arrays yet
                              1   // decl line -- change when we read this when parsing
                            );

  return symbol_table_entry;
}

// FUNCTION CALL EXPRESSION //
void
FunctionCallExpr::print() {
  printf("%s(", this->name.c_str());
  for (size_t i = 0; i < this->args.size(); i++) {
    this->args[i]->print();
    if (i < this->args.size()-1)
      printf(", ");
  }
  printf(") ");
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

  printf("[[ intexpr val: %lld type: %s ]]", this->value, dt.c_str());
}

void
BooleanExpr::print() {
  std::string val = (this->value == true) ? "true" : "false";
  printf("[[ boolean val: %s ]]", val.c_str());
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

  printf("[[ floatexpr val: %lf type: %s ]]", this->value, dt.c_str());
}

void
Conditional::print() {
  printf("%s ", this->token.literal.c_str());
  printf("(");
  this->condition->print();
  printf(") {\n");

  if (this->consequence)
    this->consequence->print();

  if (!this->alternative)
    printf("} end [if]");
  else
    printf("} ");

  
//  printf("\n---- if/else symbol table ----\n");
//  dynamic_cast<CodeBlock*>(this->consequence.get())->symbol_table->print_elements();
//  printf("------------------------\n");

  if (this->alternative) {
    this->alternative->print();
  }
}

void
WhileLoop::print() {
  printf("%s ", this->token.literal.c_str());
  printf("(");
  this->condition->print();
  printf(") {\n");

  if (this->loop_body) {
    this->loop_body->print();
  }

  printf("} end [while]\n");

//  printf("\n---- while symbol table ----\n");
//  dynamic_cast<CodeBlock*>(this->loop_body.get())->symbol_table->print_elements();
//  printf("------------------------\n");
}

void
ForLoop::print() {
  printf("%s (\n", this->token.literal.c_str());

  printf("\t");
  this->initialization->print();
  printf("\n\t");
  this->condition->print();
  printf("\n\t");
  this->action->print();

  printf("\n) {\n");
  this->loop_body->print();
  printf("} end [for]\n");
  
//  printf("\n---- for symbol table ----\n");
//  dynamic_cast<CodeBlock*>(this->loop_body.get())->symbol_table->print_elements();
//  printf("------------------------\n");
}

void
CodeBlock::print() {
  for (size_t i = 0; i < this->body.size(); i++) {
    this->body[i]->print();
    printf("\n");
  }
}

void
VariableExpr::print() {
  std::string dt;
  switch (this->data_type) {
    case TYPE_INT:
      dt = "int";
      printf("[name: '%s' type: '%s'] ",
          this->name.c_str(), dt.c_str());
      break;
    case TYPE_FLOAT:
      dt = "float";
      printf("[name: '%s' type: '%s'] ",
          this->name.c_str(), dt.c_str());
      break;
    case TYPE_STRING:
      dt = "string";
      break;
    case TYPE_VOID:
      dt = "void";
      printf("[name: '%s' type: '%s'] ",
          this->name.c_str(), dt.c_str());
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
  printf("[ ");
  this->LHS->print();
  printf(" ]");
  
  printf(" %s ", this->op.literal.c_str());
  
  // printf("rhs: ");
  printf("[ ");
  this->RHS->print();
  printf(" ]");
}

void
Statement::print() {}

void
Expression::print() {
  
}

// Let Statements
void
LetStmt::print() {
  printf("%s ", this->token.literal.c_str());
  if (this->var_assign)
    this->var_assign->print();
  else 
    printf("invalid variable assignment\n");
}

// Creates and returns a symbol table entry from the values in the statement
std::unique_ptr<SymbolTableEntry>
LetStmt::get_st_entry() {
  auto symbol_table_entry = std::make_unique<SymbolTableEntry>(
                              dynamic_cast<VariableExpr*>(this->variable.get())->name,
                              dynamic_cast<VariableExpr*>(this->variable.get())->data_type,
                              64, // size -- 64 bits for both floats and ints
                              1,  // dimensions -- 1 because we do not parse arrays yet
                              1   // decl line -- change when we read this when parsing
                            );
  return symbol_table_entry;
}
