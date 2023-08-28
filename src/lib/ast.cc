#include "ast.hh"
#include "token.hh"

#include <locale>
#include <string>
#include <cstdio>
#include <memory>

/// UTILITY FUNCTIONS /// -- will move to utils file later
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

/// AST NODE BASE CLASS ///
void
Node::syntax_analysis() {}

/// STATEMENT BASE CLASS ///
void
Statement::print() {}
void
Statement::syntax_analysis() {}

/// EXPRESSION BASE CLASS ///
void
Expression::print() {}
void
Expression::syntax_analysis() {}

/// PROGRAM NODE ///
void
Program::print() {
  for (unsigned i = 0; i < this->program_body->body.size(); i++) {
    if (this->program_body->body[i]) this->program_body->body[i]->print();
    printf("\n");
  }
}
void
Program::syntax_analysis() {
  for (unsigned i = 0; i < this->statements.size(); i++) {
    if (this->statements[i]) this->statements[i]->syntax_analysis();
    printf("\n");
  }
}


/// RETURN STATEMENT ///
void
ReturnStmt::print() {
  printf("%s ", this->token.literal.c_str());
  this->ret_val->print();
}
void
ReturnStmt::syntax_analysis() {
  printf("return syn");
}


/// EXPRESSION STATEMENT ///
void
ExpressionStatement::print() {
  if (!this->expr) {
    printf("null expr\n");
  } else {
    this->expr->print();
  }
}
void
ExpressionStatement::syntax_analysis() {
  printf("expr statement syn\n");
}


/// FUNCTION ///
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
}
void
FunctionDecl::syntax_analysis() {
  printf("func decl syn\n");
  this->func_body->syntax_analysis();
}

// Create symbol table entry from fields in the FunctionDecl class
std::shared_ptr<SymbolTableEntry>
FunctionDecl::get_st_entry() {
  auto symbol_table_entry = std::make_shared<SymbolTableEntry>(
                              this->prototype->name,
                              this->prototype->ret_type,
                              64, // size -- 64 bits for both floats and ints
                              1,  // dimensions -- 1 because we do not parse arrays yet
                              1   // decl line -- change when we read this when parsing
                            );

  return symbol_table_entry;
}


/// FUNCTION CALL EXPRESSION ///
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
FunctionCallExpr::syntax_analysis() {
  printf("func call syn\n");
}


/// INTEGER EXPRESSION ///
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
IntegerExpr::syntax_analysis() {
  printf("int expr syn\n");
}


/// BOOLEAN EXPRESSION ///
void
BooleanExpr::print() {
  std::string val = (this->value == true) ? "true" : "false";
  printf("[[ boolean val: %s ]]", val.c_str());
}
void
BooleanExpr::syntax_analysis() {
  printf("bool expr syn\n");
}


/// IDENTIFIER EXPRESSION ///
void
IdentifierExpr::print() {
  printf("%s", this->name.c_str());
}
void
IdentifierExpr::syntax_analysis() {
  printf("ident expr syn\n");
}


/// FLOAT EXPRESSION ///
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
FloatExpr::syntax_analysis() {
  printf("float expr syn\n");
}


/// CONDITIONAL STATEMENT ///
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
Conditional::syntax_analysis() {
  printf("conditional syn\n");
}


/// WHILE LOOP STATEMENT ///
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
WhileLoop::syntax_analysis() {
  printf("while syn\n");
}


/// FOR LOOP STATEMENT ///
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
ForLoop::syntax_analysis() {
  printf("for syn\n");
}


/// CODE BLOCK STATEMENT ///
void
CodeBlock::print() {
  for (size_t i = 0; i < this->body.size(); i++) {
    this->body[i]->print();
    printf("\n");
  }
}
void
CodeBlock::syntax_analysis() {
  printf("code block syn\n");
  for (size_t i = 0; i < this->body.size(); i++) {
    if (auto let_stmt = dynamic_cast<LetStmt*>(this->body[i].get())) {
      let_stmt->syntax_analysis();
    }
  }
}


/// VARIABLE EXPRESSION ///
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
VariableExpr::syntax_analysis() {
  printf("var expr syn\n");
}


/// VARIABLE ASSIGNMENT EXPRESSION ///
void
VariableAssignment::print() {
  this->variable->print();
  printf(" %s ", this->op.literal.c_str());
  if (this->RHS)
    this->RHS->print();
}
void
VariableAssignment::syntax_analysis() {
  printf("var assign syn\n");
  if (this->RHS) {
    this->RHS->syntax_analysis();
  }
}


/// BINARY EXPRESSION ///
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
BinaryExpr::syntax_analysis() {
  printf("binary expr syn\n");
  // std::shared_ptr<Node> scope; // = std::make_shared<Node>();
  // auto code_block = dynamic_cast<CodeBlock*>(this->parent.get());
  // auto program = dynamic_cast<Program*>(this->parent.get());
  // auto scope = (program != nullptr) ? program : code_block;
//  if (dynamic_cast<CodeBlock*>(this->parent.get())) {
//    //scope = std::shared_ptr<CodeBlock>(dynamic_cast<CodeBlock*>(this->parent.get()));
//    scope = std::dynamic_pointer_cast<Node>(this->parent);
//  } else if (dynamic_cast<Program*>(this->parent.get())) {
//    // scope = std::shared_ptr<Program>(dynamic_cast<Program*>(this->parent.get()));
//    scope = std::make_shared<Node>(this->parent.get());
//  }
//  if (std::dynamic_pointer_cast<CodeBlock>(this->parent)) {
//  } else {
//    scope = std::dynamic_pointer_cast<Program>(this->parent);
//  }

  std::shared_ptr<Node> scope = this->parent;
  
  if (this->LHS) {
//    if (auto var = dynamic_cast<IdentifierExpr*>(this->LHS.get())) { // VARIABLE
//      if (code_block) {
//        if (code_block->symbol_table->find(var->name) == false) {
//          printf("Error: unknown identifier |%s|\n", var->name.c_str());
//        } else 
//          printf("found |%s|\n", var->name.c_str());
//      } else if (program) {
//        printf("program?\n");
//      } else 
//        printf("BOTH NULL\n");
//    } else {
//      printf("NOT VAR EXPR\n");
//    }
//    if (auto var = std::dynamic_pointer_cast<IdentifierExpr>(this->LHS)) {
//      if (scope->symbol_table->find(var->name) == false) {
//        printf("found var\n");
//      }
//    }
  }

  // RIGHT HAND SIDE
  if (this->RHS) {
//    if (auto var = dynamic_cast<IdentifierExpr*>(this->RHS.get())) { // VARIABLE
//      if (code_block) {
//        if (code_block->symbol_table->find(var->name) == false) {
//          printf("Error: unknown identifier |%s|\n", var->name.c_str());
//        } else 
//          printf("found |%s|\n", var->name.c_str());
//      } else if (program) {
//        printf("program?\n");
//      } else 
//        printf("BOTH NULL\n");
//    } else if (auto func_call = dynamic_cast<FunctionCallExpr*>(this->RHS.get())) { // FUNCTION CALL
//      if (code_block) {
//        if (code_block->symbol_table->find(func_call->name) == false) {
//          printf("Error: unknown identifier |%s|\n", func_call->name.c_str());
//        } else {
//          printf("found func call |%s|\n", func_call->name.c_str());
//        }
//      } else if (program) {
//        printf("program?\n");
//      } 
//    }
  } 

  /*
  // Check for compatible data types
  if (this->LHS->data_type == TYPE_VOID) {
    // identifier -- lookup in symbol table
  }
  if (this->RHS->data_type == TYPE_VOID) {
    // identifier -- lookup in symbol table
  }

  if (this->LHS->data_type != this->RHS->data_type) {
    printf("ERROR: unmatched data types %s -- %s\n", get_data_type(this->LHS->data_type).c_str(), get_data_type(this->RHS->data_type).c_str());
  } else {
    printf("%s -- %s\n", get_data_type(this->LHS->data_type).c_str(), get_data_type(this->RHS->data_type).c_str());
  }
  */
}


/// LET STATEMENT ///
void
LetStmt::print() {
  printf("%s ", this->token.literal.c_str());
  if (this->var_assign)
    this->var_assign->print();
  else 
    printf("invalid variable assignment\n");
}

// Perform syntax analysis on the variable declaration
void
LetStmt::syntax_analysis() {
  printf("let syn\n");
  this->var_assign->syntax_analysis();
}

// Creates and returns a symbol table entry from the values in the statement
std::shared_ptr<SymbolTableEntry>
LetStmt::get_st_entry() {
  auto symbol_table_entry = std::make_shared<SymbolTableEntry>(
                              dynamic_cast<VariableExpr*>(this->variable.get())->name,
                              dynamic_cast<VariableExpr*>(this->variable.get())->data_type,
                              64, // size -- 64 bits for both floats and ints
                              1,  // dimensions -- 1 because we do not parse arrays yet
                              1   // decl line -- change when we read this when parsing
                            );
  return symbol_table_entry;
}


/// ABSTRACT SYNTAX TREE CLASS ///

// Iterate through the AST and do syntax analysis checks
// 1: check if identifier exists in scope or up its parent scopes symbol tables
bool
AST::syntax_analysis() {
  std::shared_ptr<Program> root = this->program_node;
  for (unsigned i = 0; i < root->statements.size(); i++) {
    root->statements[i]->syntax_analysis();
  }

  return true;
}
