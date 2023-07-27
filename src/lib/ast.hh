/*
 * ast.hh
 *
 * This file contains the structure for the abstract syntax tree of ThunderBird
 */

#pragma once
#ifndef AST_
#define AST_

#include "token.hh"
#include <vector>
#include <string>
#include <memory>

// Node in the AST
class Node {
  public:
    virtual ~Node() = default;
    virtual void print() {};
};

// Statement Node
// Does not contain a value, and excutes a command
class Statement : public Node {
  public:
    virtual ~Statement() = default;
    void print() override;
};

// Expression node
// Evaluates to a value that will be held
class Expression : public Node {
  public:
    DataType data_type;
    virtual ~Expression() = default;
    void print() override;
};

// Statement node for an expression
// Wrapper so that something like "x + 15;" is valid code on its own
class ExpressionStatement : public Statement {
  public:
    token_t token;                    // first token of the expression
    std::unique_ptr<Expression> expr; // holds the expression

    ExpressionStatement(
      token_t token,
      std::unique_ptr<Expression> expr
    ) : token(token) , expr(std::move(expr)) {}
};

//// Expression with an infix operator
//class BinaryExpr : public Expression {
//  public:
//    token_t op;
//    std::unique_ptr<Expression> LHS;
//    std::unique_ptr<Expression> RHS;
//    BinaryExpr(
//        token_t op,
//        std::unique_ptr<Expression> LHS,
//        std::unique_ptr<Expression> RHS
//      ) : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
//    void print() override;
//};

// Prefix or unary operator
class PrefixOperator {
  public:
    token_t token;
    std::string op;
    std::unique_ptr<Expression> RHS;
};

// Node for assigning a variable
// "x = 3 + 20"
class VariableAssignment : public Expression {
  public:
  token_t op; // "="
  std::unique_ptr<Expression> variable;
  std::unique_ptr<Expression> RHS;
  VariableAssignment(
      token_t op,
      std::unique_ptr<Expression> variable,
      std::unique_ptr<Expression> RHS
    ) : op(op), variable(std::move(variable)), RHS(std::move(RHS)) {}
  void print() override;
};

// Integer Expression Node
// Node that is a integer literal like "1" or "300"
class IntegerExpr : public Expression {
  public:
    long long value;
    DataType data_type = TYPE_INT;
    IntegerExpr(long long value) : value(value) {};
    void print() override;
};

// Float Expression Node
// Node that is a floating point literal like "1.0" or "3.14"
class FloatExpr : public Expression {
  public:
    double value;
    DataType data_type = TYPE_FLOAT;
    FloatExpr(double value) : value(value) {};
    void print() override;
};

// Identifier class that holds the name and data type of the identifier
// identifier can be either a function or a variable
class IdentifierExpr : public Expression {
  public:
    std::string name;   // name of the identifie
    DataType data_type; // data type of the identifier (return type for function, stored type for variable)

    void print() override;
};

// Variable Expression node
// Is an expression because a variable does contain a value that it is assigned to
class VariableExpr : public Expression {
  public:
    std::string name;   // name of the variable
    DataType data_type; // data type of the variable -- float or int
    long long value;    // integer value that the variable can hold
    double dvalue;      // floating point value that the variable can hold

    VariableExpr(const std::string &name, DataType data_type) : name(name), data_type(data_type) {}
    void print() override;
};

// Statement node for let statements for variable declaration
// "let x = 3;"
class LetStmt : public Statement {
  public:
    token_t token;                          // "let" token
    std::unique_ptr<Expression> var_assign; // expression that variable will be assigned to
   
    LetStmt(
        token_t token,
        // Expression *var_assign
        std::unique_ptr<Expression> var_assign
      ) : token(token), /*variable(std::move(variable)),*/ var_assign(std::move(var_assign)) {}
    void print() override;
};


// Function prototype statement node
// add(...)
class Prototype : public Statement {
  public:
    std::string name;              // name of the function prototype
    DataType ret_type;             // return type of the function
    std::vector <IdentifierExpr> args; // list of arguments in the function prototype -- func(int param1...)

    Prototype(std::string name, DataType ret_type,  std::vector<IdentifierExpr> args)
      : name(name), ret_type(ret_type), args(std::move(args)) {}
    void print() override;
};

// Program Node in the AST
// should be the root node of the tree
class Program : public Node {
  public:
    std::vector<std::unique_ptr<Statement> > statements; // top level of the program is a list of statements
    void print() override;
};

#endif
