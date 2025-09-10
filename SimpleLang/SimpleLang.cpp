#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "lexer.hpp"

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

class SimpleLang {
private:
  std::string filename;
  Lexer lexer;
  SymbolTable symbols;

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

  void ProcessVar() {
    lexer.Use(Lexer::ID_VAR, "Internal Compiler Error!");
    Token id_token = lexer.Use(Lexer::ID_ID);
    std::string var_name = id_token.lexeme;
    symbols.AddSymbol(id_token);
    lexer.Use('=');
    int value = GetTokenValue(lexer.Use());
    symbols.SetSymbol(var_name, value);
  }

  void ProcessPrint() {
    lexer.Use(Lexer::ID_PRINT, "Internal Compiler Error!");
    auto token = lexer.Use();
    std::cout << GetTokenValue(token) << std::endl;
  }

  void ProcessLine() {
    auto token = lexer.Peek();
    switch (token) {
      case Lexer::ID_VAR: ProcessVar(); break;
      case Lexer::ID_PRINT: ProcessPrint(); break;
      case ';': break;
      default: lexer.Error("Unknown token '", token.lexeme, "'.");
    }
    lexer.Use(';');
  }

public:
  SimpleLang(std::string filename) : filename(filename) { }

  void Run() {
    std::ifstream fs(filename);
    lexer.Tokenize(fs);

    while (lexer.Any()) {
      ProcessLine();
    }
  }
};

int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cerr << "Must have exactly 1 argument for filename.\n";
    exit(1);
  }

  std::string filename = argv[1];
  SimpleLang lang(filename);
  lang.Run();
}