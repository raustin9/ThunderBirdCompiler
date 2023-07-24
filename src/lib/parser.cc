#include "parser.hh"
#include "ast.hh"
#include "token.hh"
#include "lexer.hh"

#include <string>
#include <cstdlib>
#include <memory>
#include <type_traits>

// Constructor
Parser::Parser(std::string input) {
  this->lex = new Lexer(input);

  // Initialize the token values
  this->current_token.type = TOK_ILLEGAL;
  this->current_token.literal = "";

  this->peek_token.type = TOK_ILLEGAL;
  this->peek_token.literal = "";

  // fill in the first two tokens
  this->next_token();
  this->next_token();
}

// Advance the current token to the next one
void
Parser::next_token() {
  this->current_token = this->peek_token;
  this->peek_token = this->lex->next_token();
}

// Parse let statemetns
// "let int x = 5;"
std::unique_ptr<Statement>
Parser::parse_let_statement() {
  token_t let_tok = this->current_token;
  this->next_token(); // eat the TOK_LET token
  token_t tok = this->current_token;
  
  std::unique_ptr<Expression> assignment_expr;
  switch(tok.type) {
    case TOK_TYPEINT:
      printf("matched int\n");

      assignment_expr = this->parse_assignment(TYPE_INT);
      break;
    case TOK_TYPEFLOAT:
      printf("matched float\n");

      assignment_expr = this->parse_assignment(TYPE_FLOAT);
      break;
    default:
      printf("error: unexpected token |%s|\n", tok.literal.c_str());
      return nullptr;
      break;
  }

  auto let_statement = std::make_unique<LetStmt>(let_tok, std::move(assignment_expr));
  return let_statement;
}

// Parse variable assignment expression
// "let x = 3;"
std::unique_ptr<Expression>
Parser::parse_assignment(DataType data_type) {
  this->next_token(); // eat the Type Identifier
  token_t tok = this->current_token;

  std::string var_name = tok.literal;
  if (tok.type != TOK_IDENT) {
    printf("error: unexpected token |%s|\n", tok.literal.c_str());
    return nullptr;
  }
  printf("matched identifier\n");
  auto variable = std::make_unique<VariableExpr>(tok.literal, data_type);
  
  this->next_token(); // eat the variable
  tok = this->current_token;

  token_t op;
  if (tok.type != TOK_EQUALS) {
    printf("error: unexpected token |%s|\n", tok.literal.c_str());
    return nullptr;
  } else {
    op = tok;
  }

  this->next_token(); // eat the TOK_EQUALS
  std::unique_ptr<Expression> expr_val;
  switch (this->current_token.type) {
    case TOK_INT:
      printf("matched int\n");
      expr_val = this->parse_integer();
      break;
    case TOK_FLOAT:
      printf("matched float\n");
      expr_val = this->parse_float();
      break;
    default:
      printf("unexpected token: |%s|\n", this->current_token.literal.c_str());
      return nullptr;
  }

  if (variable->data_type == TYPE_INT) {
    auto int_expr = dynamic_cast<IntegerExpr*>(expr_val.get());
    if (variable->data_type != int_expr->data_type) {
      printf("error: wrong data dype. %s is type 'int' but got '%d'\n", variable->name.c_str(), int_expr->data_type);
      return nullptr;
    }
    variable->value = int_expr->value;
  } else if (variable->data_type == TYPE_FLOAT) {
    auto float_expr = dynamic_cast<FloatExpr*>(expr_val.get());
    if (variable->data_type != float_expr->data_type) {
      printf("error: wrong data dype. %s is type 'float' but got '%d'\n", variable->name.c_str(), float_expr->data_type);
      return nullptr;
    }
    variable->dvalue = float_expr->value;
  } else {
    printf("error: invalid data type '%d'\n", variable->data_type);
    return nullptr;
  }

//  // Ensure matching data types
//  if (variable->data_type != expr_val->data_type) {
//    printf("WRONG DT, %d != %d\n", variable->data_type, expr_val->data_type);
//    printf("expr val %d\n", dynamic_cast<FloatExpr*>(expr_val.get())->data_type);
//    variable->value = dynamic_cast<IntegerExpr*>(expr_val.get())->value;
//  }

  auto assignment_expr = std::make_unique<VariableAssignment>(op, std::move(variable), std::move(expr_val));
  return assignment_expr;
}

// Parse an integer expression
// "3" "700";
std::unique_ptr<Expression>
Parser::parse_integer() {
  token_t tok = this->current_token;
  if (tok.type != TOK_INT) {
    printf("error: unexpected token |%s|\n", tok.literal.c_str());
    return nullptr;
  }

  long long val = atoi(tok.literal.c_str());
  printf("matched int: val = %lld\n", val);

  return std::make_unique<IntegerExpr>(val);
}

std::unique_ptr<Expression>
Parser::parse_float() {
  token_t tok = this->current_token;
  if (tok.type != TOK_FLOAT) {
    printf("error: unexpected token |%s|. Expected TOK_FLOAT\n", tok.literal.c_str());
    return nullptr;
  }

  double val = atof(tok.literal.c_str());
  printf("matched float: val %lf\n", val);

  auto rv = std::make_unique<FloatExpr>(val);
  rv->data_type = TYPE_FLOAT;
  printf("rv dt %d\n", rv->data_type);
  return rv;
}

// parse the program
// start at top level and cascade down the tree
std::unique_ptr<Program>
Parser::parse_program() {
  auto program = std::make_unique<Program>();
  // std::unique_ptr<Program> program;

  // main loop
  while(this->current_token.type != TOK_EOF) {
    std::unique_ptr<Statement> stmt;
    switch(this->current_token.type) {
      case TOK_LET:
        printf("matched let\n");
        stmt = this->parse_let_statement();
        program->statements.push_back(std::move(stmt));
        this->next_token();
        // goto exit_loop;
        break;
      default:
        printf("token: %s\n", this->current_token.literal.c_str());
        this->next_token();
        break;
    }
  }
  // exit_loop:

  printf(" -- Program --\n");
  program->print();
  return program;
}
