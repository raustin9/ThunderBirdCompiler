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
Node::_syntax_analysis() {}

void
Node::_set_parent(Node* p) {}

/// STATEMENT BASE CLASS ///
void
Statement::_print() {}
void
Statement::_syntax_analysis() {}

/// EXPRESSION BASE CLASS ///
void
Expression::_print() {}
void
Expression::_syntax_analysis() {}

/// PROGRAM NODE ///
void
Program::_print() {
    for (unsigned i = 0; i < this->statements.size(); i++) {
        if (this->statements[i]) this->statements[i]->_print();
        printf("\n");
    }
}

// Recurse down the AST and assign the parents of AST nodes
// to what they need to be
void
Program::assign_parents() {
    this->parent = nullptr;
    for (size_t i = 0; i < this->statements.size(); i++) {
        if (this->statements[i]) this->statements[i]->_set_parent(this);
    }
}

// Recurse down the AST and perform syntax analysis on
// the AST nodes
void
Program::_syntax_analysis() {
    for (unsigned i = 0; i < this->statements.size(); i++) {
        if (this->statements[i]) this->statements[i]->_syntax_analysis();
    }

    if (this->entry_point == nullptr) {
        printf("Error: no entry point defined\n");
    }
}

// Search for the identifier in the program's symbol table
std::shared_ptr<SymbolTableEntry>
Program::_scope_lookup(std::string name) {
    if (this->symbol_table->find(name) == false)
        return nullptr;

    return this->symbol_table->elements[name];
}

// Set the entry point of the program to the desired function
// If there is already an entry point defined, then more than one 
// entry point is defined in the source code, and we should flag 
// an error
void
Program::_set_entry(FunctionDecl* entry_point) {
    if (this->entry_point != nullptr) {
        printf("Error: multiple entry points defined: |%s|\n", entry_point->prototype->name.c_str());
        return;
    }

    this->entry_point = entry_point;
    return;
}


/// RETURN STATEMENT ///
void
ReturnStmt::_print() {
    printf("%s ", this->token.literal.c_str());
    this->ret_val->_print();
}

// Set the parent of the return statement to the parameter
// Set the parent of the return statement's return value to the parameter as well
// Set the parent function that the return statement corresponds to
void
ReturnStmt::_set_parent(Node* p) {
    this->parent = p;
    if(this->ret_val)
        this->ret_val->_set_parent(p);


    // Go up the parent chain until
    // Find the function that this belongs to
    Node* cur = p;
    while (
        dynamic_cast<Statement*>(cur)->parent != nullptr
    ) {
        if (dynamic_cast<FunctionDecl*>(cur)) {
            printf("func got\n");
            break;
        }
            
        cur = dynamic_cast<Statement*>(cur)->parent;
    }
    this->parent_func = dynamic_cast<FunctionDecl*>(cur);
}

// Check that the data type of this return statement's expression
// is the same as the function that it returns to
void
ReturnStmt::_syntax_analysis() {
    printf("return syn\n");

    if (this->ret_val) {
        this->ret_val->_syntax_analysis();

        if (
                this->ret_val->_get_type()
                != this->parent_func->prototype->ret_type
             ) {
            printf("Error: return type '%s' does not match declared return type '%s'\n",
                    get_data_type(this->ret_val->_get_type()).c_str(),
                    get_data_type(this->parent_func->prototype->ret_type).c_str());
        } else
            printf("Return Match: '%s' == '%s'\n",
                    get_data_type(this->ret_val->_get_type()).c_str(),
                    get_data_type(this->parent_func->prototype->ret_type).c_str());
    }
}


/// EXPRESSION STATEMENT ///
void
ExpressionStatement::_print() {
    if (!this->expr) {
        printf("null expr\n");
    } else {
        this->expr->_print();
    }
}

// Set the parent of the expression statement to the parameter
// This parameter should be a CodeBlock
void
ExpressionStatement::_set_parent(Node* p) {
    if(this->expr)
        this->expr->_set_parent(p);
    // this->parent = p;
}

// Perform syntax analysis on the statement's expression value
void
ExpressionStatement::_syntax_analysis() {
    printf("expr statement syn\n");
    if (this->expr)
        this->expr->_syntax_analysis();
}


/// FUNCTION ///
void
FunctionDecl::_print() {
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
        this->func_body->_print();

    printf("\n} end [%s]\n", this->prototype->name.c_str());
}

// Set the parent of the function [should be the program]
// Set the parent of the function's body to the function itself
// Perform the _set_parent on the body of the function
void
FunctionDecl::_set_parent(Node* p) {
    this->parent = dynamic_cast<Program*>(p);
    if(this->func_body) {
        this->func_body->parent = this;
        this->func_body->_set_parent(p);
    }

    if (this->is_entry) {
        this->parent->_set_entry(this);
    }
}

// Perform syntax analysis on the function body
// [FUTURE]: perform syntax check on the function prototype
void
FunctionDecl::_syntax_analysis() {
    printf("func decl syn\n");
    if (this->func_body)
        this->func_body->_syntax_analysis();
}

// Create symbol table entry from fields in the FunctionDecl class
std::shared_ptr<SymbolTableEntry>
FunctionDecl::_get_st_entry() {
    auto symbol_table_entry = std::make_shared<SymbolTableEntry>(
                                                            this->prototype->name,
                                                            this->prototype->ret_type,
                                                            64, // size -- 64 bits for both floats and ints
                                                            1,    // dimensions -- 1 because we do not parse arrays yet
                                                            1     // decl line -- change when we read this when parsing
                                                        );

    return symbol_table_entry;
}


/// FUNCTION CALL EXPRESSION ///
void
FunctionCallExpr::_print() {
    printf("%s(", this->name.c_str());
    for (size_t i = 0; i < this->args.size(); i++) {
        this->args[i]->_print();
        if (i < this->args.size()-1)
            printf(", ");
    }
    printf(") ");
}

// Set the parent of this expression to the set parameter
void
FunctionCallExpr::_set_parent(Node* p) {
    this->parent = p;
}

// Get the data type of the function call
// [THIS MIGHT NEED TO BE SYMBOL TABLE LOOKUP -- TEST]
DataType
FunctionCallExpr::_get_type() {
    std::shared_ptr<SymbolTableEntry> func_ident = this->parent->_scope_lookup(this->name);
    if (func_ident == nullptr)
        return TYPE_VOID;

    return func_ident->data_type;
}

// [FUTURE: PERFORM SYNTAX CHECK ON FUNCTION CALL]
void
FunctionCallExpr::_syntax_analysis() {
    printf("func call syn\n");
}


/// INTEGER EXPRESSION ///
void
IntegerExpr::_print() {
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

// Return the data type of this expression
// [should always be TYPE_INT]
DataType
IntegerExpr::_get_type() {
    return this->data_type;
}

// Set the parent of the integer expression [should be a CodeBlock]
void
IntegerExpr::_set_parent(Node* p) {
    this->parent = p;
}

// Integer expr syntax check
void
IntegerExpr::_syntax_analysis() {
    printf("int expr syn\n");
}


/// FLOAT EXPRESSION ///
void
FloatExpr::_print() {
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

// Return the data type of the expression
// [should always be TYPE_FLOAT]
DataType
FloatExpr::_get_type() {
    return this->data_type;
}

// Set the parent of the expression
void
FloatExpr::_set_parent(Node* p) {
    this->parent = p;
}

// Perform syntax check on the expression
void
FloatExpr::_syntax_analysis() {
    printf("float expr syn\n");
}

/// BYTE EXPRESSION ///
void
ByteExpr::_print() {
    std::string dt;
    switch (this->data_type) {
        case TYPE_INT:
            dt = "int";
            break;
        case TYPE_BYTE:
            dt = "byte";
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

    printf("[[ byte val: %lld type: %s ]]", this->value, dt.c_str());
}

// Get the data type of this expression
// [should always be TYPE_BYTE]
DataType
ByteExpr::_get_type() {
    return this->data_type;
}

// Set the parent of the byte expression
// [should always be CodeBlock]
void
ByteExpr::_set_parent(Node* p) {
    this->parent = p;
}

// Perform syntax check on the expression
void
ByteExpr::_syntax_analysis() {
    printf("byte expr syn\n");
}


/// BOOLEAN EXPRESSION ///
void
BooleanExpr::_print() {
    std::string val = (this->value == true) ? "true" : "false";
    printf("[[ boolean val: %s ]]", val.c_str());
}

// Return data type of the expression
// [should always be TYPE_BOOL]
DataType
BooleanExpr::_get_type() {
    return this->data_type;
}

// Set the parent of the expression
// [should always be CodeBlock]
void
BooleanExpr::_set_parent(Node* p) {
    this->parent = p;
}

// Perform syntax check on the expression
void
BooleanExpr::_syntax_analysis() {
    printf("bool expr syn\n");
}


/// IDENTIFIER EXPRESSION ///
void
IdentifierExpr::_print() {
    printf("%s", this->name.c_str());
}

// Create a symbol table entry out of an identifier expression
// using its fields
// This will be used to insert into a symbol table
std::shared_ptr<SymbolTableEntry>
IdentifierExpr::_get_st_entry() {
    auto ste = std::make_shared<SymbolTableEntry>(
        this->name,
        this->data_type,
        64,
        1,
        1
    );

    return ste;
}

// Return the data type of the identifier
// This is done by doing a lookup of the identifier in its scope's 
// symbol table and go upwards to the program scope.
// If it is found, return the data type, if not, return TYPE_VOID
DataType
IdentifierExpr::_get_type() {
    // Find the identifier in symbol table and 
    // return its data type

    std::shared_ptr<SymbolTableEntry> ident = this->parent->_scope_lookup(this->name);
    if (ident == nullptr) {
        return TYPE_VOID;
    }
    return ident->data_type;
}

// Sets the parent of the identifier
void
IdentifierExpr::_set_parent(Node* p) {
    this->parent = p;
}

// Performs syntax check on the identifier
// This is done by doing a lookup of the identifier in its scope's
// symbol table, and seeing if it is there or not
void
IdentifierExpr::_syntax_analysis() {
    printf("ident expr syn\n");
    // lookup identifier in symbol table to see if it is there
    auto ident_ste = this->parent->_scope_lookup(this->name);
    if (!ident_ste) {
        printf("Error: identifier |%s| not found in this scope\n", this->name.c_str());
    } else {
        printf("Found: ident |%s|\n", this->name.c_str());
    }
}


/// CONDITIONAL STATEMENT ///
void
Conditional::_print() {
    printf("%s ", this->token.literal.c_str());
    printf("(");
    this->condition->_print();
    printf(") {\n");

    if (this->consequence)
        this->consequence->_print();

    if (!this->alternative)
        printf("} end [if]");
    else
        printf("} ");

    if (this->alternative) {
        this->alternative->_print();
    }
}

// Set the parent of the Conditional [should be either Program or CodeBlock]
// Set the parent of the condition     [should be either Program or CodeBlock]
// Set the parent of the consuence to the Conditional
// Perform the _set_parent logic down the chain on the body
// Loop until there is not alternative clause in the chain and do the same for each
void
Conditional::_set_parent(Node* p) {
    Conditional *cur = this;
    this->parent = p;
    this->condition->_set_parent(p);
    this->consequence->parent = this;
    this->consequence->_set_parent(p);

    while (cur->alternative) {
        printf("got alt\n");
        cur = dynamic_cast<Conditional*>(cur->alternative.get());
        cur->parent = p;
        cur->condition->_set_parent(p);
        cur->consequence->parent = this;
        cur->consequence->_set_parent(p);
    }
}

// Perform syntax check on the Conditional
// Check the condition, consequence, and all 
// other clauses in the chain
void
Conditional::_syntax_analysis() {
    printf("conditional syn\n");
    if (this->condition)
        this->condition->_syntax_analysis();

    if (this->consequence)
        this->consequence->_syntax_analysis();

    // this->alternative->_syntax_analysis();
    Conditional* cur = this;
    while (cur->alternative) {
        cur = dynamic_cast<Conditional*>(cur->alternative.get());
        if (cur->condition)
            cur->condition->_syntax_analysis();
        if (cur->consequence)
            cur->consequence->_syntax_analysis();
    }
}


/// WHILE LOOP STATEMENT ///
void
WhileLoop::_print() {
    printf("%s ", this->token.literal.c_str());
    printf("(");
    if (this->condition) {
        this->condition->_print();
    }
    printf(") {\n");

    if (this->loop_body) {
        this->loop_body->_print();
    }

    printf("} end [while]\n");
}

// Set the parent of the while loop [should be CodeBlock]
// Set the parent of the condition    [should be CodeBlock]
// Set the parent of the loop_body to the WhileLoop itself
// Continue _set_parent chain through the body
void
WhileLoop::_set_parent(Node* p) {
    this->parent = p;
    if(this->condition)
        this->condition->_set_parent(p);
    if(this->loop_body) {
        this->loop_body->parent = this;
        this->loop_body->_set_parent(p);
    }
}

// Perform syntax check on the condition
// and body of the while loop
void
WhileLoop::_syntax_analysis() {
    printf("while syn\n");
    if (this->condition)
        this->condition->_syntax_analysis();

    if (this->loop_body)
        this->loop_body->_syntax_analysis();
}


/// FOR LOOP STATEMENT ///
void
ForLoop::_print() {
    printf("%s (\n", this->token.literal.c_str());

    printf("\t");
    this->initialization->_print();
    printf("\n\t");
    this->condition->_print();
    printf("\n\t");
    this->action->_print();

    printf("\n) {\n");
    this->loop_body->_print();
    printf("} end [for]\n");
}

// Set the parent of the ForLoop [should be CodeBlock]
// Set the parent of the initialization [should be CodeBlock]
// Set the parent of the condition to the body of the for loop
// Set the parent of the action to the body of the for loop
// Set the parent of the body to the ForLoop itself
// Continue _set_parent chain through the body
void
ForLoop::_set_parent(Node* p) {
    this->parent = dynamic_cast<CodeBlock*>(this);
    if(this->loop_body) {
        this->loop_body->parent = this;
        this->loop_body->_set_parent(p);
    }
    if(this->initialization)
        this->initialization->_set_parent(p);
    if(this->condition)
        this->condition->_set_parent(dynamic_cast<CodeBlock*>(this->loop_body.get()));
    if(this->action)
        this->action->_set_parent(dynamic_cast<CodeBlock*>(this->loop_body.get()));
}

// Perform syntax check on the initialization,
// condition, action, and body of the loop
void
ForLoop::_syntax_analysis() {
    printf("for syn\n");
    if (this->initialization)
        this->initialization->_syntax_analysis();
    if(this->condition)
        this->condition->_syntax_analysis();
    if(this->action)
        this->action->_syntax_analysis();
    if(this->loop_body)
        this->loop_body->_syntax_analysis();
}


/// CODE BLOCK STATEMENT ///
void
CodeBlock::_print() {
    for (size_t i = 0; i < this->body.size(); i++) {
        this->body[i]->_print();
        printf("\n");
    }
}

// Set the parent scope of the code block [either CodeBlock or Program]
// Perform _set_parent chain on all contained statements
void
CodeBlock::_set_parent(Node* p) {
//    if (dynamic_cast<Program*>(p))
//        this->parent_scope = dynamic_cast<Program*>(p);
//    else
//        this->parent_scope = dynamic_cast<CodeBlock*>(p);
    this->parent_scope = p;
    for (size_t i = 0; i < this->body.size(); i++) {
        if (this->body[i])
            this->body[i]->_set_parent(this);
    }
}

// Perform syntax check on all contained statements in
// its body
void
CodeBlock::_syntax_analysis() {
    printf("code block syn\n");
    for (size_t i = 0; i < this->body.size(); i++) {
        if(this->body[i])
            this->body[i]->_syntax_analysis();
    }
}

// Lookup the name in the symbol table of this scope 
// if name is found, return the entry corresponding to it
// otherwise, search the symbol table of the parent
std::shared_ptr<SymbolTableEntry>
CodeBlock::_scope_lookup(std::string name) {
    CodeBlock* current_block = this;                                                                 // current block we are looking at
    std::shared_ptr<SymbolTable> current_table = this->symbol_table; // table of the current scope we are examining

    // Loop until we either find the identifier or exhaust all scopes
    while (1) {
        if (current_table->find(name)) {
            // Found the identifier
            return current_table->elements[name];
        } else {
            if(dynamic_cast<Program*>(current_block->parent_scope)) {
                printf("CodeBlock::_scope_lookup() -- Program Parent switch\n");
                // Parent scope is the Program Scope
                auto final_scope = dynamic_cast<Program*>(current_block->parent_scope);
                return final_scope->_scope_lookup(name);
            } else {
                printf("CodeBlock::_scope_lookup() -- CB Parent switch\n");
                // Set the current scope to its parents scope
                current_block = dynamic_cast<CodeBlock*>(current_block->parent_scope);
                current_table = current_block->symbol_table;
            }
        }
    }
}


/// VARIABLE EXPRESSION ///
void
VariableExpr::_print() {
    std::string dt;
    switch (this->data_type) {
        case TYPE_INT:
            dt = "int";
            printf("[name: '%s' type: '%s'] ",
                    this->name.c_str(), dt.c_str());
            break;
        case TYPE_BYTE:
            dt = "byte";
            printf("[name: '%s' type: '%s'] ",
                    this->name.c_str(), dt.c_str());
            break;
        case TYPE_FLOAT:
            dt = "float";
            printf("[name: '%s' type: '%s'] ",
                    this->name.c_str(), dt.c_str());
            break;
        case TYPE_BOOL:
            dt = "bool";
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

// Get the data type of the variable
DataType
VariableExpr::_get_type() {
    return this->data_type;
}

// Set the parent of the variable
void
VariableExpr::_set_parent(Node* p) {
    this->parent = p;
}

// [FUTURE: perform syntax check on variable]
void
VariableExpr::_syntax_analysis() {
    printf("var expr syn\n");
}


/// VARIABLE ASSIGNMENT EXPRESSION ///
void
VariableAssignment::_print() {
    this->variable->_print();
    printf(" %s ", this->op.literal.c_str());
    if (this->RHS)
        this->RHS->_print();
}

// Set the parent of the variable [should be either CodeBlock or Program]
// Set the parent of the assigned expression [should be either CodeBlock or Program]
void
VariableAssignment::_set_parent(Node* p) {
    this->parent = p;
    if(this->RHS)
        this->RHS->_set_parent(p);
}

// Perform syntax check on the assignment
// Check that the type of the assigned expression 
// matches the declared data type of the variable
void
VariableAssignment::_syntax_analysis() {
    printf("var assign syn\n");
    if (this->RHS) {
        this->RHS->_syntax_analysis();
        if (this->variable && this->variable->_get_type() != this->RHS->_get_type()) {
            printf("Error: invalid variable assignment: type incompatibility %s != %s\n", get_data_type(this->variable->_get_type()).c_str(), get_data_type(this->RHS->_get_type()).c_str());
        }
    }
}

// Return the data type of the assigned expression
DataType
VariableAssignment::_get_type() {
    return this->data_type;
}


/// BINARY EXPRESSION ///
void
BinaryExpr::_print() {
    // printf("lhs: ");
    printf("[ ");
    this->LHS->_print();
    printf(" ]");
    
    printf(" %s ", this->op.literal.c_str());
    
    // printf("rhs: ");
    printf("[ ");
    this->RHS->_print();
    printf(" ]");
}

// Check if the data types of the left and right hand sides 
// are the same type. If they are, return that type. 
// If not, return TYPE_VOID
// [FUTURE]
//        TYPE_FLOAT, TYPE_INT, and TYPE_BYTE are all valid types to 
//        be operated on together, so perform casting rather than saying
//        it is invalid.
DataType
BinaryExpr::_get_type() {
    DataType rhs_type = TYPE_VOID, lhs_type = TYPE_VOID;

    if (this->LHS)
        lhs_type = this->LHS->_get_type();

    if (this->RHS)
        rhs_type = this->RHS->_get_type();

    if (rhs_type == lhs_type) {
        return rhs_type;
    } else {
        printf("Error: BinaryExpr::_get_type(): %s != %s\n", get_data_type(lhs_type).c_str(), get_data_type(rhs_type).c_str());
        return TYPE_VOID;
    }
}

// Set the parent of the expression and left/right hand sides
// [should be CodeBlock]
void
BinaryExpr::_set_parent(Node* p) {
    this->parent = p;
    if(this->LHS)
        this->LHS->_set_parent(p);
    if(this->RHS)
        this->RHS->_set_parent(p);
}

// Syntax check the binary expression
// Perform checks on the left and right hand sides
// Check that the types of the left and right hand sides are the same
void
BinaryExpr::_syntax_analysis() {
    printf("binary expr syn\n");

    // bool rhs_check = false, lhs_check = false;
    if (this->LHS) {
        this->LHS->_syntax_analysis();
    }

    // RIGHT HAND SIDE
    if (this->RHS) {
        this->RHS->_syntax_analysis();
    } 

    // Check for compatible data types
    if (this->LHS && this->RHS) {
        if (this->LHS->_get_type() != this->RHS->_get_type()) {
            printf("Error: BinaryExpr::_syntax_analysis() -- %s != %s\n", get_data_type(this->LHS->_get_type()).c_str(), get_data_type(this->RHS->_get_type()).c_str());
        } else {
            printf("typematch %s == %s\n", get_data_type(this->LHS->_get_type()).c_str(), get_data_type(this->RHS->_get_type()).c_str());
        }
    }
}


/// LET STATEMENT ///
void
LetStmt::_print() {
    printf("%s ", this->token.literal.c_str());
    if (this->var_assign)
        this->var_assign->_print();
    else 
        printf("invalid variable assignment\n");
}

// Set the parent of the variable being assigned 
// and the expression it is being assigned to
// [should be either CodeBlock or Program]
void
LetStmt::_set_parent(Node* p) {
    if(this->variable)
        this->variable->_set_parent(p);
    if(this->var_assign)
        this->var_assign->_set_parent(p);
}


// Perform syntax analysis on the variable declaration
void
LetStmt::_syntax_analysis() {
    printf("let syn\n");
    if(this->var_assign)
        this->var_assign->_syntax_analysis();
}

// Creates and returns a symbol table entry from the values in the statement
std::shared_ptr<SymbolTableEntry>
LetStmt::_get_st_entry() {
    auto symbol_table_entry = std::make_shared<SymbolTableEntry>(
        dynamic_cast<VariableExpr*>(this->variable.get())->name,
        dynamic_cast<VariableExpr*>(this->variable.get())->data_type,
        64, // size -- 64 bits for both floats and ints
        1,    // dimensions -- 1 because we do not parse arrays yet
        1     // decl line -- change when we read this when parsing
    );
    return symbol_table_entry;
}


/// ABSTRACT SYNTAX TREE CLASS ///

// Iterate through the AST and do syntax analysis checks
// 1: check if identifier exists in scope or up its parent scopes symbol tables
bool
AST::_syntax_analysis() {
    std::shared_ptr<Program> root = this->program_node;
    if (root)
        root->_syntax_analysis();
//    for (unsigned i = 0; i < root->statements.size(); i++) {
//        if(root->statements[i])
//            root->statements[i]->_syntax_analysis();
//    }

    return true;
}
