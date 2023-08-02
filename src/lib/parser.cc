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
  this->has_entry = false;

  // Initialize the token values
  this->current_token.type = TOK_ILLEGAL;
  this->current_token.literal = "";

  this->peek_token.type = TOK_ILLEGAL;
  this->peek_token.literal = "";

  // Set operator precedences
  this->operator_precedences[TOK_EQUALS] = 1;
  this->operator_precedences[TOK_EQUALTO] = 2;
  this->operator_precedences[TOK_LT] = 3;
  this->operator_precedences[TOK_GT] = 3;
  this->operator_precedences[TOK_LTEQUALTO] = 3;
  this->operator_precedences[TOK_GTEQUALTO] = 3;
  this->operator_precedences[TOK_PLUS] = 4;
  this->operator_precedences[TOK_MINUS] = 4;
  this->operator_precedences[TOK_ASTERISK] = 5;
  this->operator_precedences[TOK_SLASH] = 5;
  this->operator_precedences[TOK_BANG] = 6; // future: potentialy as bitwise not '~' prefix operator of SAME precedence as the bang operator


  // fill in the first two tokens
  this->next_token();
  this->next_token();
}

// Destructor -- delete the lexical analyzer
Parser::~Parser() {
  delete this->lex;
}

// Advance the current token to the next one
void
Parser::next_token() {
  this->current_token = this->peek_token;
  this->peek_token = this->lex->next_token();
}

// Parse let statements
// This statement is where variables are declared and initialized
// Variables are required to be initialized to a specific value
// "let int x = 5;"
std::unique_ptr<Statement>
Parser::parse_let_statement() {
  token_t let_tok = this->current_token;
  this->next_token(); // eat the TOK_LET token
  token_t type_spec = this->current_token;
  DataType data_type;
  
  // Ensure valid type spec
  switch(type_spec.type) {
    case TOK_TYPEINT:
      data_type = TYPE_INT;
      break;
    case TOK_TYPEFLOAT:
      data_type = TYPE_FLOAT;
      break;
    case TOK_TYPEBYTE:
      data_type = TYPE_BYTE;
      break;
    default:
      printf("error: invalid type specifier |%s|\n", type_spec.literal.c_str());
      return nullptr;
  }

  // Parse the identifier
  this->next_token(); // eat the type specifier
  token_t ident_tok = this->current_token; // grab the identifier
  if (ident_tok.type != TOK_IDENT) {
    printf("error: unexpected token |%s|. Expected |TOK_IDENT|\n", ident_tok.literal.c_str());
    return nullptr;
  }

  this->next_token(); // eat the identifier
  if (this->current_token.type == TOK_SEMICOLON) {
    // Simple var declaration
    auto variable = std::make_unique<VariableExpr>(ident_tok.literal, data_type);
    this->next_token(); // eat the semicolon
    
    return std::make_unique<LetStmt>(let_tok, std::move(variable), nullptr);
  } else if (this->current_token.type == TOK_EQUALS) {
    // Variable declaration and assignment
    token_t op = this->current_token;
    this->next_token(); // eat the '='
    auto expr_val = this->parse_expression_interior(); // get the expression being set to the variable
    auto variable = std::make_unique<VariableExpr>(ident_tok.literal, data_type);
    auto variable_for_assign = std::make_unique<VariableExpr>(ident_tok.literal, data_type);
    auto assignment_expr = std::make_unique<VariableAssignment>(op, std::move(variable_for_assign), std::move(expr_val));

    return std::make_unique<LetStmt>(let_tok, std::move(variable), std::move(assignment_expr));
  } else {
    printf("error: unexpected token |%s|. Expected |=|\n", this->current_token.literal.c_str());
    return nullptr;
  }
}

// Parse an integer expression -- really just an integer literal
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

// Parse a float expression -- really just a float literal
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

// Parse return statements from a function body
// "return 0;"
std::unique_ptr<Statement>
Parser::parse_return_statement() {
  token_t return_tok = this->current_token;
  this->next_token(); // eat return token
                      
  auto return_val = this->parse_expression_interior(); // get the expression it is returning
  this->next_token(); // eat the ';'
  return std::make_unique<ReturnStmt>(return_tok, std::move(return_val));
}

// Parse a function definition
// functions are required to be defined where they are declared,
// so when we parse the prototype, the rest of the definition must follow
std::unique_ptr<Statement>
Parser::parse_function_defn() {
  bool is_entry = false;
  token_t decl_keyword = this->current_token;
  this->next_token(); // eat the "function" or "define" or "entry" keyword

  if (decl_keyword.type == TOK_ENTRY && this->has_entry == false)
    is_entry = true;

  // GET TYPE SPECIFIER //
  token_t tok = this->current_token;
  DataType rt;
  if (tok.type == TOK_TYPEINT) {
    rt = TYPE_INT;
  } else if (tok.type == TOK_TYPEFLOAT) {
    rt = TYPE_FLOAT;
  } else if (tok.type == TOK_TYPEBYTE) {
    rt = TYPE_BYTE;
  } else {
    printf("error: unexpected token '%s'\n. Expected 'int' or 'float'", tok.literal.c_str());
  }
  printf("matched int\n");

  this->next_token(); // eat the type specifier
              
  // GET IDENTIFIER //
  token_t ident = this->current_token;
  if (ident.type != TOK_IDENT) {
    printf("error: unexpected token '%s'. Expected IDENT\n", ident.literal.c_str());
    return nullptr;
  }

  std::string proto_name = ident.literal;
  this->next_token(); // eat the identifier

  // PARSE FUNCTION PARAMETERS //
  if (this->current_token.type != TOK_LPAREN) {
    printf("error: unexpected token '%s'. Expected '('\n", this->current_token.literal.c_str());
  }
  this->next_token(); // eat the '('
  std::vector<IdentifierExpr> params;
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

    IdentifierExpr identifier;
    identifier.name = param_name.literal;
    identifier.data_type = (param_type.type == TOK_TYPEINT) ? (TYPE_INT) : (TYPE_FLOAT);
    params.push_back(identifier);

    this->next_token(); // eat the identifier
    if (this->current_token.type != TOK_COMMA) {
      if (this->current_token.type == TOK_RPAREN)
        break;

      printf("error: unexpected token '%s'. Expected ','\n", this->current_token.literal.c_str());
    } else {
      this->next_token(); // eat the ','
    }
  }
  this->next_token(); // eat the ')' at end of parameter list

  // FUNCTION BODY //
  if (this->current_token.type == TOK_LBRACE) {
    bool has_return = false;
    std::vector <std::unique_ptr<Statement> > func_body;
    this->next_token(); // eat the '{'
 
    while (this->current_token.type != TOK_RBRACE) {
      std::unique_ptr<Statement> stmt;
      switch (this->current_token.type) {
        case TOK_LET:
          printf("let token: ||%s||\n", this->current_token.literal.c_str());
          stmt = this->parse_let_statement();
          func_body.push_back(std::move(stmt));
          this->next_token();
          break;
        case TOK_RETURN:
          // parse return statements
          printf("retur token: ||%s||\n", this->current_token.literal.c_str());
          stmt = this->parse_return_statement();
          func_body.push_back(std::move(stmt));
          has_return = true;
          while (this->current_token.type != TOK_RBRACE) this->next_token();
          break;
        default:
          printf("default token: ||%s||\n", this->current_token.literal.c_str());
          stmt = this->parse_expression_statement();
          func_body.push_back(std::move(stmt));
          break;
      }
    }
    auto proto = std::make_unique<Prototype>(proto_name, rt, params);
    if (!has_return) {
      printf("error: function %s does not have return statement\n", proto->name.c_str());
      return nullptr;
    }       
    return std::make_unique<FunctionDecl>(is_entry, std::move(proto), std::move(func_body));
  } else {
    printf("error: unexpected token '%s'. Expected '{'\n", this->current_token.literal.c_str());
    return nullptr;
  }
}

// Parse an identifier in an expression
std::unique_ptr<Expression>
Parser::parse_identifier() {
  auto ident = std::make_unique<IdentifierExpr>();
  ident->name = this->current_token.literal;
  return ident;
}

// Parse parts in an expression
std::unique_ptr<Expression>
Parser::parse_primary() {
  switch(this->current_token.type) {
    case TOK_INT:
      printf("matched %s\n", this->current_token.literal.c_str());
      return this->parse_integer();
    case TOK_FLOAT:
      printf("matched %s\n", this->current_token.literal.c_str());
      return this->parse_float();
    case TOK_IDENT:
      printf("matched %s\n", this->current_token.literal.c_str());
      return this->parse_identifier();
    case TOK_LPAREN:
      printf("matched %s\n", this->current_token.literal.c_str());
      return this->parse_parentheses_expr();
    default:
      printf("primary null\n");
      return nullptr;
  }
}

// Parse expressions within parentheses
// x + (5 * 2)
std::unique_ptr<Expression>
Parser::parse_parentheses_expr() {
  this->next_token(); // eat the '('

  auto expr = this->parse_expression_interior();

  if (!expr) {
    printf("parse_paren: null\n");
    return nullptr;
  }

  if (this->current_token.type != TOK_RPAREN) {
    printf("parse_paren: expected ')'\n");
    return nullptr;
  } else {
    printf("matched ')'\n");
  }
  // this->next_token(); // eat the ')'

  return expr;
}

// Parses expressions that are not top level
std::unique_ptr<Expression>
Parser::parse_expression_interior() {
  auto LHS = this->parse_primary();
  if (!LHS) {
    printf("parse_expr_interior: LHS null\n");
    return nullptr;
  }

  this->next_token();
  LHS = this->parse_expr(0, std::move(LHS));
  
  return LHS;
}

// Parses the expression statement wrapper
std::unique_ptr<Statement>
Parser::parse_expression_statement() {
  auto LHS = this->parse_primary();
  this->next_token();
  LHS = this->parse_expr(0, std::move(LHS));

  auto stmt = std::make_unique<ExpressionStatement>(this->current_token, std::move(LHS));

  if (this->current_token.type == TOK_SEMICOLON) {
    this->next_token();
  }

  return stmt;
}

// Get the precedence of the current token
// if it is not a valid operator it returns -1
int
Parser::get_token_precedence() {
  int tok_prec = this->operator_precedences[this->current_token.type];
  if (tok_prec <= 0) 
    return -1;

  return tok_prec;
}

std::unique_ptr<Expression>
Parser::parse_expr(int precedence, std::unique_ptr<Expression> LHS) {
  while (1) {
    if (this->current_token.type == TOK_SEMICOLON || this->current_token.type == TOK_RPAREN) {
      if (this->current_token.type == TOK_SEMICOLON) printf("semicolon\n");
      else printf("rparen\n");
      return LHS;
    }
    int prec = this->get_token_precedence();
    if (prec < precedence) {
      printf("prec %d\n", prec);
      return LHS;
    }

    printf("prec: '%s' %d\n", this->current_token.literal.c_str(), prec);

    token_t op = this->current_token;
    this->next_token();
  

    printf("op %s\n", op.literal.c_str());

    auto RHS = this->parse_primary();
    if (!RHS) {
      printf("null\n");
      return nullptr;
    }

    this->next_token();
    if (this->current_token.type == TOK_SEMICOLON || this->current_token.type == TOK_RPAREN) {
      if (this->current_token.type == TOK_SEMICOLON) printf("semicolon\n");
      else printf("rparen\n");
 
      LHS = std::make_unique<BinaryExpr>(op, std::move(LHS), std::move(RHS));
      return LHS;
    }
    int next_prec = this->get_token_precedence();
    // int next_prec = this->operator_precedences[this->current_token.type];
    printf("next prec: '%s' %d\n", this->current_token.literal.c_str(), next_prec);
    if (prec < next_prec) {
      printf("prec < next_prec\n");
      RHS = this->parse_expr(prec+1, std::move(RHS));
      if (!RHS) {
        printf("rhs null\n");
        return nullptr;
      }
    }

    // LHS = std::make_unique<BinaryExpr>(op, std::move(LHS), std::move(RHS));
    LHS = std::make_unique<BinaryExpr>(op, std::move(LHS), std::move(RHS));
  }

  return std::make_unique<Expression>();
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
        stmt = this->parse_function_defn();
        program->statements.push_back(std::move(stmt));
        this->next_token();
        break;
      case TOK_ENTRY:
        if (!this->has_entry) {
          printf("matched entry\n");
          stmt = this->parse_function_defn();
          program->statements.push_back(std::move(stmt));
          this->next_token();
        } else {
          // FUTURE: make it so that it will still parse the function correctly for error handling, but 
          // change it so that it is not considered an entry point
          printf("error: program cannot contain more than one entry point");
          stmt = this->parse_function_defn();
          this->next_token();
        }
        break;
      default:
        stmt = this->parse_expression_statement();
        program->statements.push_back(std::move(stmt));
        break;
    }
  }

  printf(" -- Program --\n");
  program->print();
  return program;
}
