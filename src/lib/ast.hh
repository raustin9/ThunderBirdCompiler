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

class Node {
  public:
    virtual ~Node() = default;
};

class Expression : public Node {
  public:

};

class Statement : public Node {
  public:

};

// Node that is a integer literal like "1" or "300"
class IntegerExpr : public Expression {
  public:
    long long value;
    IntegerExpr(long long value) : value(value) {};
};

class VariableExpr : public Expression {
  public:
    std::string name; // name of the variable
    DataType data_type;
    long long value;

    VariableExpr(const std::string &name, DataType data_type) : name(name), data_type(data_type) {}
};

class LetStmt : public Statement {
  public:
    token_t token;
    std::unique_ptr<VariableExpr> variable;
    std::unique_ptr<IntegerExpr> value;
    LetStmt(
        token_t token,
        std::unique_ptr<VariableExpr> variable,
        std::unique_ptr<IntegerExpr> value
      ) : token(token), variable(std::move(variable)), value(std::move(value)) {}
};

class Program : public Node {
  public:
    std::vector<std::unique_ptr<Statement> > statements;
};

//enum NodeType {
//  EXPR,
//  STMT,
//  PRGM
//};
//
//class Node {
//  virtual token_t token_literal();
//  NodeType type;
//};
//
//// Holds the value of an expression
//// Expressions can hold the value of a 64bit integer, 64 bit float (double) 
//union Value {
//  int int_value;
//  double float_value;
//};
//
//class Expression : public Node {
//  public:
//    NodeType type = EXPR;
//    Value expr_value;
//};
//
//class Identifier : public Expression {
//  public:
//    NodeType type = EXPR;
//    token_t token;      // should be TOK_IDENT
//    Value value;   // value that the identifier holds
//    DataType data_type; // the data type of the identifier -- INT vs DOUBLE etc
//};
//
//// Statement -- a line of code that does not contain a value
//// ie let a = 1;
////    continue;
////    while...
////    etc
//class Statement : public Node{
//  public:
//    NodeType type = STMT;
//    token_t token;
//};
//
//// Let clause
//// -- "let a = 2 + 8;" 
//class LetStatement : public Statement {
//  public:
//    token_t token;         // should be TOK_LET
//    Identifier var_name;   // the variable being set in the let statement
//    Expression expr_value; // the expression that the variable is being set to
//};
//
//// Top level node of the AST
//// The actual ThunderBird programs are 
//class Program : public Node {
//  public:
//    token_t token_literal() override;
//    NodeType type = PRGM;
//    std::vector<Statement> statements;
//};

#endif
