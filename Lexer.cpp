#include "Lexer.h"

#include <cctype>
#include <cstdlib>

std::ifstream SourceFile;
double NumVal;

int gettok() {
  static int LastChar = ' ';

  while (isspace(LastChar))
    LastChar = SourceFile.get();

  if (isdigit(LastChar) || LastChar == '.') {
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = SourceFile.get();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0);
    return TOK_NUMBER;
  }

  if (LastChar == EOF)
    return TOK_EOF;

  int ThisChar = LastChar;
  LastChar = SourceFile.get();
  return ThisChar;
}
