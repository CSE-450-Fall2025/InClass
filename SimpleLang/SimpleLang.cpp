#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "lexer.hpp"
#include "ASTNode.hpp"
#include "SymbolTable.hpp"

using emplex::Lexer;
using emplex::Token;

class SimpleLang {
private:
  std::string filename;
  Lexer lexer;
  SymbolTable symbols;

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

  // void ProcessVar() {
  //   lexer.Use(Lexer::ID_VAR, "Internal Compiler Error!");
  //   Token id_token = lexer.Use(Lexer::ID_ID);
  //   std::string var_name = id_token.lexeme;
  //   symbols.AddSymbol(id_token);
  //   lexer.Use('=');
  //   int value = GetTokenValue(lexer.Use());
  //   symbols.SetSymbol(var_name, value);
  // }

  // void ProcessPrint() {
  //   lexer.Use(Lexer::ID_PRINT, "Internal Compiler Error!");
  //   auto token = lexer.Use();
  //   std::cout << GetTokenValue(token) << std::endl;
  // }

  // void ProcessLine() {
  //   auto token = lexer.Peek();
  //   switch (token) {
  //     case Lexer::ID_VAR: ProcessVar(); break;
  //     case Lexer::ID_PRINT: ProcessPrint(); break;
  //     case ';': break;
  //     default: lexer.Error("Unknown token '", token.lexeme, "'.");
  //   }
  //   lexer.Use(';');
  // }

  ASTNode Parse_Term() {
    int next_token = lexer.Peek();
    switch (next_token) {
    case Lexer::ID_ID:
      return ASTNode(lexer.Use());
    case Lexer::ID_NUMBER:
      return ASTNode(lexer.Use());
    default:
      lexer.Error("Unexpected Token");
    }

    // SHOULD NEVER GET HERE!
    return ASTNode(Token{Lexer::ID__EOF_, "ERROR", 0});
  }

  ASTNode Parse_Expression() {
    ASTNode expr_node = Parse_Term();

    if (lexer.Peek() == Lexer::ID_OP) {
      Token op_token = lexer.Use();
      ASTNode term2 = Parse_Term();
      expr_node = ASTNode(op_token, expr_node, term2);
    }

    return expr_node;
  }

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
    int next_token = lexer.Peek();
    ASTNode statement_node;
    switch (next_token) {
      case Lexer::ID_VAR:   statement_node = Parse_StatementVAR();   break;
      case Lexer::ID_PRINT: statement_node = Parse_StatementPRINT(); break;
      default:
        lexer.Error("Unexpected Token");
    }

    lexer.Use(';');
    return statement_node;
  }

public:
  SimpleLang(std::string filename) : filename(filename) { }

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