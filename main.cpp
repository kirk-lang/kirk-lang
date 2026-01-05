#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "Lexer.h"
#include "Parser.h"
#include <iostream>

using namespace llvm;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: kirk <filename.kirk>\n";
        return 1;
    }

    SourceFile.open(argv[1]);
    if (!SourceFile.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << "\n";
        return 1;
    }
    
    auto AST = Parse();

    if (AST) {
        std::cout << "Parsed successfully! The AST is built.\n";
    } else {
        std::cerr << "Error parsing file.\n";
    }

    return 0;
}

