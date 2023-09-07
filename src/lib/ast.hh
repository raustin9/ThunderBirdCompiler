/*
 * ast.hh
 *
 * This file contains the structure for the abstract syntax tree of ThunderBird
 */

#pragma once
#ifndef AST_
#define AST_

#include "token.hh"
#include "symboltable.hh"
#include "errorhandler.hh"
#include <vector>
#include <string>
#include <memory>

// The Abstract Syntax Tree itself
class AST {
    public:
        std::shared_ptr<class Program> program_node;
        bool _syntax_analysis(); // iterates over the tree and performs syntax analysis
};

// Node in the AST
class Node {
    public:
        virtual ~Node() = default;
        virtual void _print() {};
        virtual void _syntax_analysis();
        virtual void _set_parent(Node* p);
        virtual std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) {return nullptr;}
};

// Statement Node
// Does not contain a value, and excutes a command
class Statement : public Node {
    public:
        virtual ~Statement() = default;
        void _print() override;
        Node* parent;
        void _syntax_analysis() override;
        void _set_parent(Node* p) override {}
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
};

// Expression node
// Evaluates to a value that will be held
class Expression : public Node {
    public:
        Node* parent;
        // std::shared_ptr<Node> parent;
        DataType data_type;
        virtual ~Expression() = default;
        virtual DataType _get_type() {return TYPE_VOID;};
        void _print() override;
        void _syntax_analysis() override;
        void _set_parent(Node* p) override {}
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
};

// Statement node for an expression
// Wrapper so that something like "x + 15;" is valid code on its own
class ExpressionStatement : public Statement {
    public:
        token_t token;                                        // first token of the expression
        std::shared_ptr<Expression> expr; // holds the expression

        ExpressionStatement(
            token_t token,
            std::shared_ptr<Expression> expr
        ) : token(token) , expr(std::move(expr)) {}
        void _print() override;
        void _syntax_analysis() override;
        void _set_parent(Node* p) override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
};

//// Expression with an infix operator
class BinaryExpr : public Expression {
    public:
        token_t op;
        std::shared_ptr<Expression> LHS;
        std::shared_ptr<Expression> RHS;
        BinaryExpr(
                token_t op,
                std::shared_ptr<Expression> LHS,
                std::shared_ptr<Expression> RHS
            ) : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
        DataType _get_type() override;
};

// Prefix or unary operator
class PrefixOperator {
    public:
        token_t token;
        std::string op;
        std::shared_ptr<Expression> RHS;
};

// Node for assigning a variable
// "x = 3 + 20"
class VariableAssignment : public Expression {
    public:
    token_t op; // "="
    std::shared_ptr<Expression> variable;
    std::shared_ptr<Expression> RHS;
    VariableAssignment(
            token_t op,
            std::shared_ptr<Expression> variable,
            std::shared_ptr<Expression> RHS
        ) : op(op), variable(std::move(variable)), RHS(std::move(RHS)) {}
    void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
        DataType _get_type() override;
};

// Integer Expression Node
// Node that is a integer literal like "1" or "300"
class IntegerExpr : public Expression {
    public:
        long long value;
        // DataType data_type = TYPE_INT;
        IntegerExpr(long long value) : value(value) {};
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
        DataType _get_type() override;
};

// Byte Expression Node
// Node that is a byte literal -- this is something like 'a' in the future
//                                                                but will just be a 8 bit number for now
class ByteExpr : public Expression {
    public:
        long long value;
        // DataType data_type = TYPE_INT;
        ByteExpr(long long value) : value(value) {};
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
        DataType _get_type() override;
};

// Float Expression Node
// Node that is a floating point literal like "1.0" or "3.14"
class FloatExpr : public Expression {
    public:
        double value;
        // DataType data_type = TYPE_FLOAT;
        FloatExpr(double value) : value(value) {};
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
        DataType _get_type() override;
};

// Statement node for code block
// Code block is contained within '{}'
// {
//     let int x = 1;
// }
class CodeBlock : public Statement {
    public:
        std::vector <std::shared_ptr<Statement> > body; // the body of code of this scope
        std::shared_ptr<SymbolTable> symbol_table;            // the symbol table of identifiers for this code block's scope
        // std::shared_ptr<Node> parent_scope;                // the parent scope of this code block. Can be function or global scope
        Node* parent_scope;
        // add a field for an inner scope

        CodeBlock() {
            this->symbol_table = std::make_shared<SymbolTable>();
        }
        void _print() override;
        void _syntax_analysis() override;
        void print_st();
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override; // lookup the name in the scope
        void _set_parent(Node* p) override;
};

// Boolean Expression Node
// Node that is a boolean value like 'true' or 'false'
class BooleanExpr : public Expression {
    public:
        bool value;
        // DataType data_type = TYPE_BOOL;

        BooleanExpr(
            bool value
        ) : value(value) {}
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
        DataType _get_type() override;
};

// Function Call Expression node
// This node is for when a function is called in an expression
// "func1(x, y);"
// "let int x = add(1, 2) + 4;"
class FunctionCallExpr : public Expression {
    public:
        std::string name;
        std::vector <std::shared_ptr<Expression> > args;

        FunctionCallExpr(
            std::string &name,
            std::vector <std::shared_ptr<Expression> > args
        ) : name(name), args(std::move(args)) {}
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
        DataType _get_type() override;
};

// Identifier class that holds the name and data type of the identifier
// identifier can be either a function or a variable
class IdentifierExpr : public Expression {
    public:
        std::string name;     // name of the identifie
        // DataType data_type; // data type of the identifier (return type for function, stored type for variable)

        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
        std::shared_ptr<SymbolTableEntry> _get_st_entry();
        DataType _get_type() override;
};

// Variable Expression node
// Is an expression because a variable does contain a value that it is assigned to
class VariableExpr : public Expression {
    public:
        std::string name;        // name of the variable
        DataType data_type;    // data type of the variable -- float or int
        Node *parent;

        VariableExpr(const std::string &name, DataType data_type) : name(name), data_type(data_type) {}
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
        DataType _get_type() override;
};

// Statement node for let statements for variable declaration
// "let x = 3;"
class LetStmt : public Statement {
    public:
        token_t token;                                                    // "let" token
        std::shared_ptr<Expression> variable;     // expression of the variable being declared
        unsigned decl_line;                                         // the line of the statement
        std::shared_ptr<Expression> var_assign; // expression that variable will be assigned to
     
        LetStmt(
                token_t token,
                std::shared_ptr<Expression> variable,
                std::shared_ptr<Expression> var_assign
            ) : token(token), variable(std::move(variable)), var_assign(std::move(var_assign)) {}
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _get_st_entry();
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
};

// Statement node for return statements
// "return x + 5;"
class ReturnStmt : public Statement {
    public:
        token_t token;
        class FunctionDecl* parent_func;
        // std::shared_ptr<class FunctionDecl> parent_func;
        std::shared_ptr<Expression> ret_val; // return value of the function
        
        ReturnStmt(
            token_t token,
            std::shared_ptr<Expression> ret_val
        ) : token(token), ret_val(std::move(ret_val)) {}
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
};

// Statement node for if statements
// "if (x == 1) {
//        print("true");
//    } else if (x == 2) {
//        print("x = 2");
//    } else {
//        print("false");
//    }
//    Notes:
//        The field "condition" is evaluated, and if it returns 'true' then the "consequence" field is evaluated
//        If the condition returns false, then we then evaluate the condition of the "alternative" field
//        The reason that the "alternative" field is another conditional is because it allows us to chain if clauses together
//        This is how we implement "else if" statements
class Conditional : public Statement {
    public:
        token_t token;
        std::shared_ptr<Statement> consequence;                         // body of if statement
        std::shared_ptr<Expression> condition;                            // the condition to evaluate
        std::shared_ptr<Statement> alternative;                         // the conditional to evaluate if the condition is not true -- this is how we do else-if
        // std::shared_ptr<Statement> parent;                                    // parent scope of the conditional
        // Statement* parent;

        Conditional(
            token_t token,
            std::shared_ptr<Statement> consequence,
            std::shared_ptr<Expression> condition,
            std::shared_ptr<Statement> alternative
        ) : token(token), 
                consequence(std::move(consequence)),
                condition(std::move(condition)),
                alternative(std::move(alternative))
            {}
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
};

// Statement node for while loop
// This will loop while the condition remains true and break when false
// "while (x    < 4)..."
class WhileLoop : public Statement {
    public:
        token_t token;
        std::shared_ptr<Expression> condition;
        std::shared_ptr<Statement> loop_body;
        // Statement* parent;
        // std::shared_ptr<Statement> parent;        // symbol table of the loop 

        WhileLoop(
            token_t token,
            std::shared_ptr<Expression> condition,
            std::shared_ptr<Statement> loop_body
        ) : token(token), 
                condition(std::move(condition)),
                loop_body(std::move(loop_body))
            {}
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
};

// Statement node for a for loop
// This will perform the intialization before entering the loop
// Then check the condition, and if true it will run the loop body
// At end of loop body, it will perform the action
// "for (let int x = 0; x < 13; x = x + 1) {...}"
class ForLoop : public Statement {
    public:
        token_t token;                                                         // the token that represents the command
        std::shared_ptr<Statement> initialization; // the initialization statement in the for loop
        std::shared_ptr<Expression> condition;         // the condition that the loop runs until fulfilled
        std::shared_ptr<Expression> action;                // the action that gets taken at the end of each iteration
        std::shared_ptr<Statement> loop_body;            // the body of the for loop
        //std::shared_ptr<Statement> parent;                 // symbol table of the loop 

        ForLoop(
            token_t token,
            std::shared_ptr<Statement> initialization,
            std::shared_ptr<Expression> condition,
            std::shared_ptr<Expression> action,
            std::shared_ptr<Statement> loop_body
        ) : token(token) , initialization(std::move(initialization)), condition(std::move(condition)) , action(std::move(action)), loop_body(std::move(loop_body)) {}
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
};

// Class for function prototypes
class Prototype {
    public:
    std::string name;
    DataType ret_type;
    std::vector <IdentifierExpr> params;

    Prototype(
        std::string name,
        DataType ret_type,
        std::vector <IdentifierExpr> params
    ) : name(std::move(name)), ret_type(ret_type), params(std::move(params)) {}
};

// Class for function declarations
class FunctionDecl : public Statement {
    public:
        bool is_entry;                                                                 // true if it is the entry point to the program false otherwise
        std::shared_ptr<Statement> func_body;                    // a CodeBlock that contains the body of the function
        std::shared_ptr<Prototype> prototype;                    // the prototype of the function
        // std::shared_ptr<class Program> parent;                 // parent scope of the function -- global scope
        Program* parent;

        FunctionDecl(
            bool is_entry,
            std::shared_ptr<Statement> func_body,
            std::shared_ptr<Prototype> prototype
        ) : is_entry(is_entry), func_body(std::move(func_body)), prototype(std::move(prototype)) {}
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _get_st_entry();
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override {return nullptr;}
        void _set_parent(Node* p) override;
};

// Program Node in the AST
// should be the root node of the tree
class Program : public Node {
    public:
        void assign_parents(); // top down function that cascades through all nodes and assigns parents
        std::shared_ptr<Node> parent;
        FunctionDecl* entry_point; // Potentially use to define entry point of program
        std::vector<std::shared_ptr<Statement> > statements; // top level of the program is a list of statements
        std::shared_ptr<SymbolTable> symbol_table;                     // the symbol table for the global scope
        void _print() override;
        void _syntax_analysis() override;
        std::shared_ptr<SymbolTableEntry> _scope_lookup(std::string name) override; // lookup the name in the scope
        void _set_parent(Node* p) override {}
        void _set_entry(FunctionDecl* entry_point); // set the entry point of the program

        Program() {
            this->symbol_table = std::make_shared<SymbolTable>();
            this->entry_point = nullptr;
        }
};

#endif
