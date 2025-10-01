#include <string>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>

#include "lexer.hpp"
#include "ASTNode.hpp"
#include "SymbolTable.hpp"

using emplex::Lexer;
using emplex::Token;

class SimpleLang {
private:
  std::string filename;  // Source file being compiled
  Lexer lexer;           // Lexer to perform the tokenization
  SymbolTable symbols;   // Current status of all variables

  struct OpInfo {
    int prec;
    bool recurse_left = false;
    bool recurse_right = false;
  };

  std::unordered_map<std::string, OpInfo> op_map;  // Info about binary operators
  int max_prec_level = 0;                          // Max current precedence level

  // Use max int as the precedence if we don't have an operator.
  static constexpr int NO_OP_PREC = std::numeric_limits<int>::max();

  ASTNode root{Token{'{', "ROOT", 0}};  // Root is a block of code.

  int GetTokenValue(Token token) {
    if (token == Lexer::ID_ID) {
      return symbols.GetSymbolValue(token.lexeme);
    }
    if (token == Lexer::ID_NUMBER) {
      return std::stoi(token.lexeme);
    }
    std::cerr << "Unexpected token '" << token.lexeme << '\n';
    exit(1);
    return 0;
  }

  ASTNode Parse_Term() {
    Token next_token = lexer.Peek();
    switch (next_token) {
    case Lexer::ID_ID:
      return ASTNode(lexer.Use());
    case Lexer::ID_NUMBER:
      return ASTNode(lexer.Use());
    case '(': {
      lexer.Use('(');
      ASTNode result = Parse_Expression();
      lexer.Use(')');
      return result;
    }
    default:
      lexer.Error("Unexpected Token: '", next_token.lexeme, "'");
    }

    // SHOULD NEVER GET HERE!
    return ASTNode(Token{Lexer::ID__EOF_, "ERROR", 0});
  }

  // ASTNode Parse_Expr3() {
  //   ASTNode lhs = Parse_Expr2();
  //   return Parse_Expr3(lhs);
  // }

  // ASTNode Parse_Expr3(ASTNode lhs) {
  //   if (lexer.Peek().lexeme == "+" || lexer.Peek().lexeme == "-") {
  //     Token op_token = lexer.Use(Lexer::ID_OP);
  //     ASTNode rhs = Parse_Expr2();
  //     lhs = ASTNode{op_token, lhs, rhs};

  //     lhs = Parse_Expr3(lhs);
  //   }

  //   return lhs;
  // }

  // ASTNode Parse_Expr2() {
  //   ASTNode lhs = Parse_Expr1();
  //   return Parse_Expr2(lhs);
  // }

  // ASTNode Parse_Expr2(ASTNode lhs) {
  //   if (lexer.Peek().lexeme == "*" || lexer.Peek().lexeme == "/") {
  //     Token op_token = lexer.Use(Lexer::ID_OP);
  //     ASTNode rhs = Parse_Expr1();
  //     lhs = ASTNode{op_token, lhs, rhs};

  //     lhs = Parse_Expr2(lhs);
  //   }

  //   return lhs;
  // }
  // ASTNode Parse_Expr1() {
  //   ASTNode lhs = Parse_Term();

  //   if (lexer.Peek().lexeme == "**") {
  //     Token op_token = lexer.Use(Lexer::ID_OP);
  //     ASTNode rhs = Parse_Expr1();
  //     lhs = ASTNode{op_token, lhs, rhs};
  //   }

  //   return lhs;
  // }

  OpInfo GetOpInfo(Token token) {
    auto it = op_map.find(token.lexeme);
    if (it == op_map.end()) return OpInfo{NO_OP_PREC};
    return it->second;
  }

  // ASTNode Parse_Expression() { return Parse_Expr3(); }

  ASTNode Parse_Expression(int prec_level) {
    assert(prec_level >= 0 && prec_level <= max_prec_level);

    // If our precedence can't match operators, return a term.
    if (prec_level == 0) return Parse_Term();

    // Fill out any higher precedence on left-hand side (LHS)
    ASTNode lhs = Parse_Expression(prec_level - 1);

    // If we found an operator on THIS precedence level, act on it.
    OpInfo op_info;
    while ((op_info = GetOpInfo(lexer.Peek())).prec == prec_level) {
      Token op_token = lexer.Use();  // Get the current operator.

      // Determine the right-hand-side of this operator.
      size_t right_prec = op_info.recurse_right ? prec_level : prec_level - 1;
      ASTNode rhs = Parse_Expression(right_prec);

      // Now that we have both sides, build the node!
      ASTNode result = ASTNode{op_token, lhs, rhs};

      // If left associative, shift result to lhs and loop to put any more
      // ops at this precedence level ABOVE this one.
      if (op_info.recurse_left) lhs = result;
      else return result; // ...otherwise exit with this result.
    }

    return lhs;
  }

  // If precedence level is not specified, use max.
  ASTNode Parse_Expression() { return Parse_Expression(max_prec_level); }

    // ASTNode Parse_Expression() {
  //   ASTNode expr_node = Parse_Term();

  //   if (lexer.Peek() == Lexer::ID_OP) {
  //     Token op_token = lexer.Use();
  //     ASTNode term2 = Parse_Term();
  //     expr_node = ASTNode(op_token, expr_node, term2);
  //   }

  //   return expr_node;
  // }

  ASTNode Parse_StatementVAR() {
    lexer.Use(Lexer::ID_VAR);
    Token id_token = lexer.Use(Lexer::ID_ID);
    ASTNode lhs_node(id_token);
    std::string var_name = id_token.lexeme;
    symbols.AddSymbol(id_token);
    Token assign_token = lexer.Use('=');
    ASTNode rhs_node = Parse_Expression();  // Right-hand side of assignment
    return ASTNode(assign_token, lhs_node, rhs_node);
  }

  ASTNode Parse_StatementPRINT() {
    Token print_token = lexer.Use(Lexer::ID_PRINT);
    ASTNode print_node = Parse_Expression();
    return ASTNode(print_token, print_node);
  }

  ASTNode Parse_Statement() {
    Token next_token = lexer.Peek();
    ASTNode statement_node;
    switch (next_token) {
      case Lexer::ID_VAR:   statement_node = Parse_StatementVAR();   break;
      case Lexer::ID_PRINT: statement_node = Parse_StatementPRINT(); break;
      default:
        statement_node = Parse_Expression();
//        lexer.Error("Unexpected Token: '", next_token.lexeme, "'");
    }

    lexer.Use(';');
    return statement_node;
  }

public:
  SimpleLang(std::string filename) : filename(filename) {
    op_map["**"] = OpInfo{++max_prec_level, false, true}; // Right-associative
    op_map["*"] = op_map["/"] = OpInfo{++max_prec_level, true, false}; // Left
    op_map["+"] = op_map["-"] = OpInfo{++max_prec_level, true, false}; // Left
    op_map["="] = OpInfo{++max_prec_level, false, true}; // Right-associative
  }

  void Parse() {
    std::ifstream fs(filename);
    lexer.Tokenize(fs);

    while (lexer.Any()) {
      ASTNode statement_node = Parse_Statement();
      root.AddChild(statement_node);
    }

    root.Run(symbols);
    // root.PrintDebug();
  }
};

int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cerr << "Must have exactly 1 argument for filename.\n";
    exit(1);
  }

  std::string filename = argv[1];
  SimpleLang lang(filename);
  lang.Parse();
}