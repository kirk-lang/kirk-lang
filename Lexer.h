#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <string>

extern std::ifstream SourceFile;
extern double NumVal;

enum Token {
  TOK_EOF = -1,
  TOK_NUMBER = -2,
  TOK_IDENTIFIER = -3, // Variable
  TOK_ASSIGN = -4
};

int gettok();

#endif
