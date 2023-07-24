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
  if (this->current_token.type == TOK_INT) {
    if (variable->data_type != TYPE_INT) {
      printf("error: invalid data type 'int'. Expected '%d'\n", variable->data_type);
      return nullptr;
    }
    expr_val = this->parse_integer();
    variable->value = dynamic_cast<IntegerExpr*>(expr_val.get())->value;
  } else if (this->current_token.type == TOK_FLOAT) {
    if (variable->data_type != TYPE_FLOAT) {
      printf("error: invalid data type 'float'. Expected '%d'\n", variable->data_type);
      return nullptr;
    }
    expr_val = this->parse_float();
    variable->dvalue = dynamic_cast<FloatExpr*>(expr_val.get())->value;
  } else {
    printf("error: inavlid data type '%s'\n", this->current_token.literal.c_str());
    return nullptr;
  }


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

// Parse a float expression
// "3.0" "700.29"
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
  return rv;
}

std::unique_ptr<Statement>
Parser::parse_prototype() {
  this->next_token(); // eat the "function"

  // Get the type specifier
  token_t tok = this->current_token;
  DataType rt;
  if (tok.type == TOK_TYPEINT) {
    rt = TYPE_INT;
  } else if (tok.type == TOK_TYPEFLOAT) {
    rt = TYPE_FLOAT;
  } else {
    printf("error: unexpected token '%s'\n", tok.literal.c_str());
  }

  this->next_token(); // eat the type specifier
              
  // get the identifier
  token_t ident = this->current_token;
  std::string proto_name = ident.literal;
  this->next_token(); // eat the identifier

  // Parse the function arguments
  if (this->current_token.type != TOK_LPAREN) {
    printf("error: unexpected token '%s'. Expected '('\n", this->current_token.literal.c_str());
  }
  this->next_token(); // eat the '('
  std::vector<Identifier> params;
  while(this->current_token.type != TOK_RPAREN) {
    token_t param_type = this->current_token;
    if (param_type.type == TOK_RPAREN) 
      break;

    this->next_token(); // eat the parameter's type spec
    token_t param_name = this->current_token;
    if (param_name.type != TOK_IDENT) {
      printf("error: unexpected token '%s'. Expected IDENT\n", param_name.literal.c_str());
      return nullptr;
    }

    Identifier identifier;
    identifier.name = param_name.literal;
    identifier.data_type = (param_type.type == TOK_TYPEINT) ? (TYPE_INT) : (TYPE_FLOAT);
    params.push_back(identifier);
  }

  return std::make_unique<Prototype>(proto_name, rt, params);
}

// parse the program
// start at top level and cascade down the tree
std::unique_ptr<Program>
Parser::parse_program() {
  auto program = std::make_unique<Program>();

  // main loop
  while(this->current_token.type != TOK_EOF) {
    std::unique_ptr<Statement> stmt;
    switch(this->current_token.type) {
      case TOK_LET: // 'let' variable assignment statement
        printf("matched let\n");
        stmt = this->parse_let_statement();
        program->statements.push_back(std::move(stmt));
        this->next_token();

        break;
      case TOK_FUNCTION:
        printf("matched function\n");
        stmt = this->parse_prototype();
        program->statements.push_back(std::move(stmt));
        break;
      default:
        printf("token: %s\n", this->current_token.literal.c_str());
        this->next_token();
        break;
    }
  }

  printf(" -- Program --\n");
  program->print();
  return program;
}
