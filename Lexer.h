#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <string>

extern std::ifstream SourceFile;
extern double NumVal;

enum Token { tok_eof = -1, tok_number = -2 };

int gettok();

#endif
