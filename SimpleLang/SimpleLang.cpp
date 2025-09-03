#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "lexer.hpp"

using emplex::Lexer;
using emplex::Token;

class SimpleLang {
private:
  std::string filename;
  Lexer lexer;

  std::map<std::string, int> symbols;  // From var name to value.

  int GetTokenValue(Token token) {
    // MAKE SURE IT EXISTS
    if (token == Lexer::ID_ID) {
      return symbols[token.lexeme];
    }
    if (token == Lexer::ID_NUMBER) {
      return std::stoi(token.lexeme);
    }
  }

  void ProcessVar() {
    lexer.Use(Lexer::ID_VAR, "Internal Compiler Error!");
    Token id_token = lexer.Use(Lexer::ID_ID);
    std::string var_name = id_token.lexeme;
    if (symbols.contains(var_name)) {
      lexer.Error("Redeclaration of variable '", var_name, "'.");
    }
    lexer.Use('=');
    int value = GetTokenValue(lexer.Use());
    symbols[var_name] = value;
  }

  void ProcessLine() {
    auto token = lexer.Peek();
    switch (token) {
      case Lexer::ID_VAR: ProcessVar(); break;
      case Lexer::ID_PRINT: ProcessPrint(); break;
      default: lexer.Error("Unknown token '", token.lexeme, "'.");
    }
  }

  void ProcessPrint() {
    lexer.Use();
    auto token = lexer.Use();
    std::cout << GetTokenValue(token) << std::endl;
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