#pragma once

#include <assert.h>
#include <vector>

#include "lexer.hpp"
#include "SymbolTable.hpp"

using emplex::Lexer;
using emplex::Token;

class ASTNode {
private:
  Token token;

  std::vector<ASTNode> children{};

public:
  ASTNode() : token(Token{'{', "Statement Block", 0}) { }
  ASTNode(Token token) : token(token) { }
  ASTNode(Token token, ASTNode child) : token(token) {
    AddChild(child);
  }
  ASTNode(Token token, ASTNode child1, ASTNode child2) : token(token) {
    AddChild(child1);
    AddChild(child2);
  }

  void AddChild(ASTNode child) {
    children.push_back(child);
  }

  void DoAssign(int value, SymbolTable & symbols) {
    if (token != Lexer::ID_ID) {
      std::cerr << "ERROR (line " << token.line_id << "): Cannot assign a value to '"
                << token.lexeme << "'." << std::endl;
      exit(1);
    }

    symbols.SetSymbol(token.lexeme, value);
  }

  void PrintDebug(std::string prefix="") const {
    std::cout << prefix << token.lexeme << std::endl;
    for (const ASTNode & child : children) {
      child.PrintDebug(prefix + "  ");
    }
  }

  int Run_Assign(SymbolTable & symbols) {
    assert(children.size() == 2);
    int rhs_value = children[1].Run(symbols);
    children[0].DoAssign(rhs_value, symbols);
    return rhs_value;
  }

  int Run_Block(SymbolTable & symbols) {
    for (ASTNode & child : children) child.Run(symbols);
    return 0;
  }

  int Run_Number([[maybe_unused]] SymbolTable & symbols) {
    return std::stoi(token.lexeme);
  }

  int Run_Print(SymbolTable & symbols) {
    for (ASTNode & child : children) std::cout << child.Run(symbols) << std::endl;
    return 0;
  }

  int Run_Variable(SymbolTable & symbols) {
    return symbols.GetSymbolValue(token.lexeme);
  }

  int Run(SymbolTable & symbols) {
    switch (token) {
    case '{': return Run_Block(symbols);
    case '=': return Run_Assign(symbols);
    case Lexer::ID_ID: return Run_Variable(symbols);
    case Lexer::ID_NUMBER: return Run_Number(symbols);
    case Lexer::ID_PRINT: return Run_Print(symbols);
    }

    std::cerr << "Internal Compiler Error!  Don't know how to run token '"
              << token.lexeme << "'." << std::endl;
    exit(2);
    return 0;
  }
};