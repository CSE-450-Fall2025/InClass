#pragma once

#include <map>

#include "lexer.hpp"
#include "ASTNode.hpp"

using emplex::Lexer;
using emplex::Token;

class SymbolTable {
private:
  struct VarInfo {
    int value;
    int def_line;
  };

  std::map<std::string, VarInfo> symbols;  // From var name to value.
public:
  void AddSymbol(Token token) {
    if (symbols.contains(token.lexeme)) {
      std::cerr << "ERROR (line " << token.line_id << "): "
        << "Redeclaration of variable '"
        << token.lexeme << "'.\n"
        << "Originally defined on line " << symbols[token.lexeme].def_line << ".\n";
      exit(1);
    }
    symbols[token.lexeme].value = 0;
    symbols[token.lexeme].def_line = token.line_id;
  }

  [[nodiscard]] int GetSymbolValue(std::string name) {
    if (!symbols.contains(name)) {
      std::cerr << "Unknown variable '" << name << "'.\n";
      exit(1);
    }
    return symbols[name].value;
  }

  void SetSymbol(std::string name, int value) {
    if (!symbols.contains(name)) {
      std::cerr << "Unknown variable '" << name << "'.\n";
      exit(1);
    }
    symbols[name].value = value;
  }
};
