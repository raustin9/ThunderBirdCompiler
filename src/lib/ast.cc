#include "ast.hh"
#include "symboltable.hh"
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
  for (unsigned i = 0; i < this->statements.size(); i++) {
    if (this->statements[i]) this->statements[i]->print();
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

// Search for the identifier in the program's symbol table
std::shared_ptr<SymbolTableEntry>
Program::scope_lookup(std::string name) {
  if (this->symbol_table->find(name) == false)
    return nullptr;

  return this->symbol_table->elements[name];
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

// Lookup the name in the symbol table of this scope 
// if name is found, return the entry corresponding to it
// otherwise, search the symbol table of the parent
std::shared_ptr<SymbolTableEntry>
CodeBlock::scope_lookup(std::string name) {
  CodeBlock* current_block = this;                                 // current block we are looking at
  std::shared_ptr<SymbolTable> current_table = this->symbol_table; // table of the current scope we are examining

  // Loop until we either find the identifier or exhaust all scopes
  while (1) {
    if (current_table->find(name)) {
      // Found the identifier
      return current_table->elements[name];
    } else {
      if(std::dynamic_pointer_cast<Program>(this->parent_scope)) {
        // Parent scope is the Program Scope
        auto final_scope = std::dynamic_pointer_cast<Program>(this->parent_scope);
        current_table = final_scope->symbol_table;
        if (current_table->find(name)) {
          // Found identifier in program scope
          return current_table->elements[name];
        } else {
          // Identifier is not in any scopes or program scope
          return nullptr;
        }

      } else {
        // Set the current scope to its parents scope
        current_block = dynamic_cast<CodeBlock*>(current_block->parent_scope.get());
        current_table = current_block->symbol_table;
      }
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

  std::shared_ptr<SymbolTable> scope = std::make_shared<SymbolTable>();
  if (dynamic_cast<CodeBlock*>(this->parent.get())) {
    scope = dynamic_cast<CodeBlock*>(this->parent.get())->symbol_table;
  } else if (dynamic_cast<Program*>(this->parent.get())) {
    scope = dynamic_cast<Program*>(this->parent.get())->symbol_table;
  }

  std::shared_ptr<SymbolTableEntry> ident;
  if (this->LHS) {
    if (auto var = std::dynamic_pointer_cast<IdentifierExpr>(this->LHS)) { // VARIABLE EXPR
      if (!(ident = this->parent->scope_lookup(var->name))) {
        // Variable name not found in this scope
        printf("Error: unknown identifier |%s| in this scope\n", var->name.c_str());
      } else {
        printf("Found: var |%s|\n", var->name.c_str());
        this->LHS->data_type = ident->data_type;
        printf("lhs dt: %s\n", get_data_type(ident->data_type).c_str());
      }
    } else if (auto func_call = std::dynamic_pointer_cast<FunctionCallExpr>(this->LHS)) {
      if (!(ident = this->parent->scope_lookup(func_call->name))) {
        printf("Error: Undefined function |%s|\n", func_call->name.c_str());
      } else {
        printf("Found: function |%s|\n", func_call->name.c_str());
        this->LHS->data_type = ident->data_type;
        printf("lhs dt: %s\n", get_data_type(ident->data_type).c_str());
      } 
    }
  }

  // RIGHT HAND SIDE
  if (this->RHS) {
    if (auto var = std::dynamic_pointer_cast<IdentifierExpr>(this->RHS)) { // VARIABLE EXPR
      if (!(ident = this->parent->scope_lookup(var->name))) {
        // Variable name not found in this scope
        printf("Error: unknown identifier |%s| in this scope\n", var->name.c_str());
      } else {
        printf("Found: var |%s|\n", var->name.c_str());
        this->LHS->data_type = ident->data_type;
        printf("lhs dt: %s\n", get_data_type(ident->data_type).c_str());
      }
    } else if (auto func_call = std::dynamic_pointer_cast<FunctionCallExpr>(this->RHS)) {
      if (!(ident = this->parent->scope_lookup(func_call->name))) {
        printf("Error: Undefined function |%s|\n", func_call->name.c_str());
      } else {
        printf("Found: function |%s|\n", func_call->name.c_str());
        this->RHS->data_type = ident->data_type;
        printf("lhs dt: %s\n", get_data_type(func_call->data_type).c_str());
      } 
    }
  } 

  // Check for compatible data types
  if (this->LHS->data_type != this->RHS->data_type) {
    printf("ERROR: unmatched data types %s -- %s\n", get_data_type(this->LHS->data_type).c_str(), get_data_type(this->RHS->data_type).c_str());
  } else {
    printf("%s -- %s\n", get_data_type(this->LHS->data_type).c_str(), get_data_type(this->RHS->data_type).c_str());
  }
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
