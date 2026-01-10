#include "Lexer.h"

#include <cctype>
#include <cstdlib>

std::ifstream SourceFile;
double NumVal;
std::string IdentifierStr;

int gettok() {
  static int LastChar = ' ';

  while (isspace(LastChar))
    LastChar = SourceFile.get();

  // Identifiers of the types: id = [_a-zA-Z][_a-zA-Z0-9]*
  if (isalpha(LastChar)) {
    IdentifierStr = LastChar;

    while (isalnum(LastChar = SourceFile.get()) || LastChar == '_') {
      IdentifierStr += LastChar;
    }

    return TOK_IDENTIFIER;
  }

  if (isdigit(LastChar) || LastChar == '.') {
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = SourceFile.get();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0);
    return TOK_NUMBER;
  }

  if (LastChar == '=') {
    LastChar = SourceFile.get();
    return TOK_ASSIGN;
  }

  if (LastChar == EOF)
    return TOK_EOF;

  int ThisChar = LastChar;
  LastChar = SourceFile.get();
  return ThisChar;
}
