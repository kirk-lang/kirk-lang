#include "Lexer.h"

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>

std::vector<std::string> SourceLines;
SourceLocation CurLoc = {1, 0};
int CurLine = 1;
int CurCol = 0;

std::ifstream SourceFile;
double NumVal;
std::string IdentifierStr;

void LogErrorAt(SourceLocation Loc, const std::string &Msg) {
  std::cerr << "Error at " << Loc.Line << ":" << Loc.Col << ": " << Msg << "\n";

  if (Loc.Line > 0 && Loc.Line <= SourceLines.size()) {
    std::string LineContent = SourceLines[Loc.Line - 1];
    std::cerr << "  " << Loc.Line << " | " << LineContent << "\n";

    // Calculate indentation for the caret
    std::string LineNumStr = std::to_string(Loc.Line);
    std::cerr << "  " << std::string(LineNumStr.length(), ' ') << " | "
              << std::string(Loc.Col - 1, ' ') << "^" << "\n";
  }
}

int gettok() {
  static int LastChar = ' ';

  static std::map<std::string, int> Keywords = {
      {"if", TOK_IF}, {"then", TOK_THEN}, {"else", TOK_ELSE}};

  while (isspace(LastChar)) {
    // Handle newlines to track line numbers
    if (LastChar == '\n') {
      CurLine++;
      CurCol = 0;
    }

    LastChar = SourceFile.get();

    // Increment column for every character read
    if (LastChar != EOF) {
      CurCol++;
    }
  }

  // Snapshot the location before parsing the token body
  CurLoc = {CurLine, CurCol};

  // Identifiers of the types: id = [a-zA-Z][a-zA-Z0-9_]*
  if (isalpha(LastChar)) {
    IdentifierStr = LastChar;

    while (isalnum(LastChar = SourceFile.get()) || LastChar == '_') {
      IdentifierStr += LastChar;
      CurCol++;
    }

    // Check for keywords
    auto It = Keywords.find(IdentifierStr);
    if (It != Keywords.end()) {
      return It->second;
    }

    return TOK_IDENTIFIER;
  }

  // Numbers: [0-9.]+
  if (isdigit(LastChar) || LastChar == '.') {
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = SourceFile.get();
      CurCol++;
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0);
    return TOK_NUMBER;
  }

  if (LastChar == '=') {
    LastChar = SourceFile.get();
    CurCol++;
    return TOK_ASSIGN;
  }

  if (LastChar == EOF)
    return TOK_EOF;

  // Handle ASCII characters
  int ThisChar = LastChar;
  LastChar = SourceFile.get();
  CurCol++;

  return ThisChar;
}
