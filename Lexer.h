#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <memory>
#include <string>
#include <vector>

struct SourceLocation {
  int Line;
  int Col;
};

extern std::ifstream SourceFile;
extern double NumVal;
extern std::string IdentifierStr;

extern std::vector<std::string> SourceLines;
extern SourceLocation CurLoc;

enum Token {
  TOK_EOF = -1,
  TOK_NUMBER = -2,
  TOK_IDENTIFIER = -3, // Variable
  TOK_ASSIGN = -4,
  TOK_THEN = -5,
  TOK_ELSE = -6,
  TOK_IF = -7,
  TOK_PRINT = -8,
  TOK_WHILE = -9,
  TOK_EQ = -10,
  TOK_NEQ = -11,
  TOK_LEQ = -12,
  TOK_GEQ = -13
};

int gettok();
void LogErrorAt(SourceLocation Loc, const std::string &Msg);

#endif
