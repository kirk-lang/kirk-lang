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
};

int gettok();
void LogErrorAt(SourceLocation Loc, const std::string &Msg);

#endif
