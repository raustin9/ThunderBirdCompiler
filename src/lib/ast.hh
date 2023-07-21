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
    virtual void print() {};
};

class Expression : public Node {
  public:
    virtual ~Expression() = default;
    void print() override;
};

class Statement : public Node {
  public:
    virtual ~Statement() = default;
    void print() override;
};

class BinaryExpr : public Expression {
  public:
    token_t op;
    std::unique_ptr<Expression> LHS;
    std::unique_ptr<Expression> RHS;
    BinaryExpr(
        token_t op,
        std::unique_ptr<Expression> LHS,
        std::unique_ptr<Expression> RHS
      ) : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
    void print() override;
};

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

// Node that is a integer literal like "1" or "300"
class IntegerExpr : public Expression {
  public:
    long long value;
    IntegerExpr(long long value) : value(value) {};
    void print() override;
};

class VariableExpr : public Expression {
  public:
    std::string name; // name of the variable
    DataType data_type;
    long long value;

    VariableExpr(const std::string &name, DataType data_type) : name(name), data_type(data_type) {}
    void print() override;
};

class LetStmt : public Statement {
  public:
    token_t token; // "let" token
    // Expression *var_assign;
    std::unique_ptr<Expression> var_assign;
    LetStmt(
        token_t token,
        // Expression *var_assign
        std::unique_ptr<Expression> var_assign
      ) : token(token), /*variable(std::move(variable)),*/ var_assign(std::move(var_assign)) {}
    void print() override;
};

class Program : public Node {
  public:
    std::vector<std::unique_ptr<Statement> > statements;
    void print() override;
};


#endif
