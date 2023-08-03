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
  printf("next_token: eating '%s'\n", this->current_token.literal.c_str());
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
  printf("parse_let: should be eating 'let'\n");
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
    case TOK_TYPEBOOL:
      data_type = TYPE_BOOL;
      break;
    default:
      printf("error: invalid type specifier |%s|\n", type_spec.literal.c_str());
      return nullptr;
  }

  // Parse the identifier
  printf("parse_let: should be eating type specifier\n");
  this->next_token(); // eat the type specifier
  token_t ident_tok = this->current_token; // grab the identifier
  if (ident_tok.type != TOK_IDENT) {
    printf("error: unexpected token |%s|. Expected |TOK_IDENT|\n", ident_tok.literal.c_str());
    return nullptr;
  }

  printf("parse_let: should be eating identifier\n");
  this->next_token(); // eat the identifier
  if (this->current_token.type == TOK_SEMICOLON) {
    // Simple var declaration
    auto variable = std::make_unique<VariableExpr>(ident_tok.literal, data_type);
    printf("parse_let: should be eating ';'\n");
    this->next_token(); // eat the semicolon
    
    return std::make_unique<LetStmt>(let_tok, std::move(variable), nullptr);
  } else if (this->current_token.type == TOK_EQUALS) {
    // Variable declaration and assignment
    token_t op = this->current_token;
    printf("parse_let: should be eating '='\n");
    this->next_token(); // eat the '='
    auto expr_val = this->parse_expression_interior(); // get the expression being set to the variable
    auto variable = std::make_unique<VariableExpr>(ident_tok.literal, data_type);
    auto variable_for_assign = std::make_unique<VariableExpr>(ident_tok.literal, data_type);
    auto assignment_expr = std::make_unique<VariableAssignment>(op, std::move(variable_for_assign), std::move(expr_val));

    if (this->current_token.type == TOK_SEMICOLON) {
      printf("parse_let: should be eating ';'\n");
      this->next_token();
    }
    else
      printf("let_stmt: curtok = '%s'\n", this->current_token.literal.c_str());
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

  printf("parse_int: should be eating int literal\n");
  this->next_token();
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

  printf("parse_float: should be eating float literal\n");
  this->next_token();
  auto rv = std::make_unique<FloatExpr>(val);
  return rv;
}

// Parse a boolean expression -- boolean literal
// "true" "false"
std::unique_ptr<Expression>
Parser::parse_boolean() {
  token_t tok = this->current_token;

  if (tok.type != TOK_TRUE && tok.type != TOK_FALSE) {
    printf("parse_boolean: error: unexepected token '%s'. Expected 'true' or 'false'\n", tok.literal.c_str());
    return nullptr;
  }

  printf("parse_boolean: should be eating 'true' or 'false'\n");
  this->next_token();

  bool val = (tok.type == TOK_TRUE) ? true : false;
  return std::make_unique<BooleanExpr>(val);
}

// Parse return statements from a function body
// "return 0;"
std::unique_ptr<Statement>
Parser::parse_return_statement() {
  token_t return_tok = this->current_token;
  printf("parse_return: should be eating 'return'\n");
  this->next_token(); // eat return token
                      
  auto return_val = this->parse_expression_interior(); // get the expression it is returning
  printf("parse_return: should be eating ';'\n");
  this->next_token(); // eat the ';'
  return std::make_unique<ReturnStmt>(return_tok, std::move(return_val));
}

// Parse an else clause within an if statement
// checks to see if there is an else followed by an if to chain them together
std::unique_ptr<Statement>
Parser::parse_else_statement() {
  return nullptr;
}

// Parse a function definition
// functions are required to be defined where they are declared,
// so when we parse the prototype, the rest of the definition must follow
std::unique_ptr<Statement>
Parser::parse_function_defn() {
  bool is_entry = false;
  token_t decl_keyword = this->current_token;
  printf("parse_func: should be eating 'function' or 'define' or 'entry'\n");
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
  } else if (tok.type == TOK_TYPEBOOL) {
    rt = TYPE_BOOL;
  } else {
    printf("error: unexpected token '%s'\n. Expected 'int' or 'float'", tok.literal.c_str());
  }
  printf("matched int\n");

  printf("parse_func: should be eating type spec\n");
  this->next_token(); // eat the type specifier
              
  // GET IDENTIFIER //
  token_t ident = this->current_token;
  if (ident.type != TOK_IDENT) {
    printf("error: unexpected token '%s'. Expected IDENT\n", ident.literal.c_str());
    return nullptr;
  }

  std::string proto_name = ident.literal;
  printf("parse_func: should be eating identifier\n");
  this->next_token(); // eat the identifier

  // PARSE FUNCTION PARAMETERS //
  if (this->current_token.type != TOK_LPAREN) {
    printf("error: unexpected token '%s'. Expected '('\n", this->current_token.literal.c_str());
  }
  printf("parse_func: should be eating '('\n");
  this->next_token(); // eat the '('
                      
  std::vector<IdentifierExpr> params;
  while(this->current_token.type != TOK_RPAREN) {
    token_t param_type = this->current_token;
    if (param_type.type == TOK_RPAREN) 
      break;

    printf("parse_func: should be eating param type spec\n");
    this->next_token(); // eat the parameter's type spec
    token_t param_name = this->current_token;
    if (param_name.type != TOK_IDENT) {
      printf("error: unexpected token '%s'. Expected IDENT\n", param_name.literal.c_str());
      return nullptr;
    }

    IdentifierExpr identifier;
    identifier.name = param_name.literal;
    if (param_type.type == TOK_TYPEINT) {
      identifier.data_type = TYPE_INT;
    } else if (param_type.type == TOK_TYPEFLOAT) {
      identifier.data_type = TYPE_FLOAT;
    } else if (param_type.type == TOK_TYPEBYTE) {
      identifier.data_type = TYPE_BYTE;
    } else if (param_type.type == TOK_TYPEBOOL) {
      identifier.data_type = TYPE_BOOL;
    } else {
      printf("error: unexpected token '%s'\n. Expected 'int' or 'float'", tok.literal.c_str());
    }
    // identifier.data_type = (param_type.type == TOK_TYPEINT) ? (TYPE_INT) : (TYPE_FLOAT);
    params.push_back(identifier);

    printf("parse_func: should be eating param identifier\n");
    this->next_token(); // eat the identifier
    if (this->current_token.type != TOK_COMMA) {
      if (this->current_token.type == TOK_RPAREN)
        break;

      printf("error: unexpected token '%s'. Expected ','\n", this->current_token.literal.c_str());
    } else {
      printf("parse_func: should be eating ','\n");
      this->next_token(); // eat the ','
    }
  }
  printf("parse_func: should be eating ')'\n");
  this->next_token(); // eat the ')' at end of parameter list

  // FUNCTION BODY //
  if (this->current_token.type == TOK_LBRACE) {
    bool has_return = false;
    std::vector <std::unique_ptr<Statement> > func_body;
    printf("parse_func: should be eating '{'\n");
    this->next_token(); // eat the '{'
 
    while (this->current_token.type != TOK_RBRACE) {
      std::unique_ptr<Statement> stmt;
      switch (this->current_token.type) {
        case TOK_LET:
          printf("let token: ||%s||\n", this->current_token.literal.c_str());
          stmt = this->parse_let_statement();
          func_body.push_back(std::move(stmt));
          break;
        case TOK_IF:
          printf("if token: ||%s||\n", this->current_token.literal.c_str());
          stmt = this->parse_if_statement();
          func_body.push_back(std::move(stmt));
          break;
        case TOK_RETURN:
          // parse return statements
          printf("retur token: ||%s||\n", this->current_token.literal.c_str());
          stmt = this->parse_return_statement();
          func_body.push_back(std::move(stmt));
          has_return = true;
          // while (this->current_token.type != TOK_RBRACE) this->next_token();
          break;
        default:
          printf("default token: ||%s||\n", this->current_token.literal.c_str());
          stmt = this->parse_expression_statement();
          func_body.push_back(std::move(stmt));
          break;
      }
    }

    printf("parse_func: should be eating '}'\n");
    this->next_token(); // eat the '}' at end of function body

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

// Parse an if statement
std::unique_ptr<Statement>
Parser::parse_if_statement() {
  token_t token = this->current_token;
  printf("if_stmt: should be eating 'if'\n");
  this->next_token(); // eat the 'if'

  if (this->current_token.type != TOK_LPAREN) {
    printf("if_stmt: error: unexpected token '%s'. Expected '('\n", this->current_token.literal.c_str());
  }

  auto condition = this->parse_expression_interior();

  if (this->current_token.type != TOK_LBRACE) {
    printf("error: unexpected token '%s'. Expected '{'\n", this->current_token.literal.c_str());
  }
  printf("if_stmt: should be eating '{'\n");
  this->next_token(); // eat the '{'

  // PARSE IF STATEMENT BODY //
  std::vector<std::unique_ptr<Statement> > consequence;
  while (this->current_token.type != TOK_RBRACE) {
    // for now: eat the body
    std::unique_ptr<Statement> stmt;
    switch (this->current_token.type) {
      case TOK_LET:
        printf("let token: ||%s||\n", this->current_token.literal.c_str());
        stmt = this->parse_let_statement();
        consequence.push_back(std::move(stmt));
        break;
      case TOK_IF:
        printf("if token: ||%s||\n", this->current_token.literal.c_str());
        stmt = this->parse_if_statement();
        consequence.push_back(std::move(stmt));
        break;
      default:
        printf("default token: ||%s||\n", this->current_token.literal.c_str());
        stmt = this->parse_expression_statement();
        consequence.push_back(std::move(stmt));
        break;
    }
  }

  printf("ifstmt: should be eating '}'\n");
  this->next_token(); // eat the '}'
  
  // PARSE ELSE CLAUSE //
  if (this->current_token.type == TOK_ELSE) {
    printf("if_stmt: contains else clause\n");

    token_t else_tok = this->current_token;
    printf("if_stmt: should be eating 'else'\n");
    this->next_token();

    if (this->current_token.type == TOK_IF) {
      // else if...
      printf("if_stmt: matched else if\n");
      auto alternative = this->parse_if_statement();

      auto if_stmt = std::make_unique<Conditional>(token, std::move(condition), std::move(consequence), std::move(alternative));
      return if_stmt;
    } else if (this->current_token.type == TOK_LBRACE) {
      // just normal else clause
      printf("if_stmt: final else clause\n");
      printf("if_stmt: else: should be eating '{'\n");
      this->next_token();

      // ELSE CLAUSE BODY //
      std::vector <std::unique_ptr<Statement> > else_body;
      while (this->current_token.type != TOK_RBRACE) {
        std::unique_ptr<Statement> stmt;
        switch (this->current_token.type) {
          case TOK_LET:
            printf("let token: ||%s||\n", this->current_token.literal.c_str());
            stmt = this->parse_let_statement();
            else_body.push_back(std::move(stmt));
            break;
          case TOK_IF:
            printf("if token: ||%s||\n", this->current_token.literal.c_str());
            stmt = this->parse_if_statement();
            else_body.push_back(std::move(stmt));
            break;
          default:
            printf("default token: ||%s||\n", this->current_token.literal.c_str());
            stmt = this->parse_expression_statement();
            else_body.push_back(std::move(stmt));
            break;
        }
      }
      auto else_condition = std::make_unique<BooleanExpr>(true);
      auto else_stmt = std::make_unique<Conditional>(else_tok, std::move(else_condition), std::move(else_body), nullptr); 
      
      printf("if_stmt: else: should be eating '}'\n");
      this->next_token();
      
      auto if_stmt = std::make_unique<Conditional>(token, std::move(condition), std::move(consequence), std::move(else_stmt));
      return if_stmt;
    }
  }


  // No else or else if clauses
  auto if_stmt = std::make_unique<Conditional>(token, std::move(condition), std::move(consequence), nullptr);
  return if_stmt;
}

// Parse an identifier in an expression
std::unique_ptr<Expression>
Parser::parse_identifier() {
  auto ident = std::make_unique<IdentifierExpr>();
  ident->name = this->current_token.literal;
  printf("parse_ident: should be eating identifier\n");
  this->next_token();
  return ident;
}

// Parse parts in an expression
std::unique_ptr<Expression>
Parser::parse_primary() {
  switch(this->current_token.type) {
    case TOK_INT:
      printf("primary matched %s\n", this->current_token.literal.c_str());
      return this->parse_integer();
    case TOK_FLOAT:
      printf("primary matched %s\n", this->current_token.literal.c_str());
      return this->parse_float();
    case TOK_TRUE:
      printf("primary matched %s\n", this->current_token.literal.c_str());
      return this->parse_boolean();
    case TOK_FALSE:
      printf("primary matched %s\n", this->current_token.literal.c_str());
      return this->parse_boolean();
    case TOK_IDENT:
      printf("primary matched %s\n", this->current_token.literal.c_str());
      return this->parse_identifier();
    case TOK_LPAREN:
      printf("primary matched %s\n", this->current_token.literal.c_str());
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
  if (this->current_token.type != TOK_LPAREN) {
    printf("parse_paren: error -- first token '%s' != ')'\n", this->current_token.literal.c_str());
  }
  printf("parse_paren: should be eating '('\n");
  this->next_token(); // eat the '('

  auto expr = this->parse_expression_interior();

  if (!expr) {
    printf("parse_paren: null\n");
    return nullptr;
  }

  if (this->current_token.type != TOK_RPAREN) {
    printf("parse_paren: expected ')' got '%s'\n", this->current_token.literal.c_str());
    return nullptr;
  } else {
    printf("parse_paren: matched ')'\n");
    this->next_token(); // eat the ')'
  }

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

  LHS = this->parse_expr(0, std::move(LHS));
  
  return LHS;
}

// Parses the expression statement wrapper
std::unique_ptr<Statement>
Parser::parse_expression_statement() {
  auto LHS = this->parse_primary();
  LHS = this->parse_expr(0, std::move(LHS));

  auto stmt = std::make_unique<ExpressionStatement>(this->current_token, std::move(LHS));

  if (this->current_token.type == TOK_SEMICOLON) {
    printf("parse_expr_stmt: should be eating ';'\n");
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
      if (this->current_token.type == TOK_SEMICOLON) printf("semicolon 1\n");
      else printf("rparen1\n");
      return LHS;
    }

    int prec = this->get_token_precedence();
    if (prec < precedence) {
      printf("prec1 '%s' = %d\n", this->current_token.literal.c_str(), prec);
      return LHS;
    }
    printf("prec: '%s' %d\n", this->current_token.literal.c_str(), prec);

    token_t op = this->current_token;
    printf("op %s\n", op.literal.c_str());

    printf("parse_expr: should be eating operator\n");
    this->next_token();
  
    auto RHS = this->parse_primary();
    if (!RHS) {
      printf("null\n");
      return nullptr;
    }

    if (this->current_token.type == TOK_SEMICOLON || this->current_token.type == TOK_RPAREN) {
      if (this->current_token.type == TOK_SEMICOLON) printf("semicolon 2\n");
      else printf("rparen2\n");
 
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

        break;
      case TOK_FUNCTION:
        printf("matched function\n");
        stmt = this->parse_function_defn();
        program->statements.push_back(std::move(stmt));
        break;
      case TOK_ENTRY:
        if (!this->has_entry) {
          printf("matched entry\n");
          stmt = this->parse_function_defn();
          program->statements.push_back(std::move(stmt));
        } else {
          // FUTURE: make it so that it will still parse the function correctly for error handling, but 
          // change it so that it is not considered an entry point
          printf("error: program cannot contain more than one entry point");
          stmt = this->parse_function_defn();
          // this->next_token();
        }
        break;
      case TOK_IF:
        printf("matched if\n");
        stmt = this->parse_if_statement();
        program->statements.push_back(std::move(stmt));
      case TOK_EOF:
        printf("parse_program: TOK_EOF in switch ending program\n");
        break;
      default:
        // invalid top level statement
        // error recovery: panic mode
        printf("error: invalid token '%s' at top level\n", this->current_token.literal.c_str());
        this->parse_expression_statement();
        break;
    }
  }
  printf("\n -- Input --\n%s\n", this->lex->input.c_str());

  printf(" -- Program --\n");
  program->print();
  return program;
}
