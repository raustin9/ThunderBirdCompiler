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


class Statement;
class Expression;

class Node {
public:
  virtual std::string token_literal();
};


// STATEMENTS //
class Statement : public Node {
public:
  std::string token_literal() override;
  virtual void statement_node();
};


class Expression : public Node {
  public:
    std::string token_literal() override;
    virtual void expression_node();
};

class Identifier : public Expression {
public:
  void expression_node() override;
  std::string token_literal() override;
  token_t token;
  std::string value;
};

// PROGRAM //
class Program : public Node {
public:
  std::string token_literal() override;
  std::vector <Statement> statements;
};

class LetStatement : public Statement{
public:
  void statement_node() override;
  std::string token_literal() override;
  token_t token;
  Identifier name;
  Expression value;
};
#endif
