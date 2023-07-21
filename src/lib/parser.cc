#include "parser.hh"
#include "ast.hh"
#include "token.hh"
#include "lexer.hh"

#include <string>
#include <cstdlib>
#include <memory>

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
    default:
      printf("error: unexpected token |%s|\n", tok.literal.c_str());
      return nullptr;
      break;
  }

  auto let_statement = std::make_unique<LetStmt>(let_tok, std::move(assignment_expr));
  return let_statement;
}

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

    default:
      printf("unexpected token: |%s|\n", this->current_token.literal.c_str());
      return nullptr;
  }

  auto assignment_expr = std::make_unique<VariableAssignment>(op, std::move(variable), std::move(expr_val));
  return assignment_expr;
}

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
  for (size_t i = 0; i < program->statements.size();  i++) {
    LetStmt* ls = dynamic_cast<LetStmt*>(program->statements[i].get());
    ls->print();
//    VariableAssignment* be = dynamic_cast<VariableAssignment*>(ls->var_assign.get());
//    printf("%s ", ls->token.literal.c_str());
//    printf("%s %s %lld\n", dynamic_cast<VariableExpr*>(be->variable.get())->name.c_str(), be->op.literal.c_str(), dynamic_cast<IntegerExpr*>(be->RHS.get())->value);
  }
  return program;
}
