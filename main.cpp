// C++ Libraries
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>

// LLVM Libraries
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

// Global State Variables
std::ifstream SourceFile;
std::string IdentifierStr;  // Word like "x" or "def"
double NumVal;              // Number like "10.5"

// Labels, like numbers or variables
enum Token {
  tok_eof = -1,         // End of File
  tok_identifier = -2,  // A variable name like "abc"
  tok_number = -3       // Any number like "1.0" or "5"
};

int gettok() {
  static int LastChar = ' ';

  // 1. Skip any whitespace
  while (isspace(LastChar))
    LastChar = SourceFile.get();

  // 2. Handle Numbers (Integer and Floating Point)
  if (isdigit(LastChar) || LastChar == '.') {
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = SourceFile.get();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0);
    return tok_number;
  }

  // 3. Handle EOF
  if (LastChar == EOF)
    return tok_eof;

  // 4. Handle Operators (return the ASCII value, e.g., '+')
  int ThisChar = LastChar;
  LastChar = SourceFile.get();
  return ThisChar;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: kirk <filename.kirk>\n";
        return 1;
    }

    // Open the file provided in the argument
    SourceFile.open(argv[1]);
    if (!SourceFile.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << "\n";
        return 1;
    }

    std::cout << "Reading file: " << argv[1] << "\n";

    // Test Loop: Print tokens to verify the Lexer works
    while (true) {
        int tok = gettok();
        
        if (tok == tok_eof) break;

        if (tok == tok_number) {
            std::cout << "Token: Number (" << NumVal << ")\n";
        } else if (tok < 0) {
             std::cout << "Token: Identifier (" << IdentifierStr << ")\n";
        } else {
            std::cout << "Token: Operator '" << (char)tok << "'\n";
        }
    }

    return 0;
}

