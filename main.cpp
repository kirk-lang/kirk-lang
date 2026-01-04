#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "Lexer.h"
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

    std::cout << "Reading file: " << argv[1] << "\n";

    while (true) {
        int tok = gettok();

        if (tok == tok_eof) break;

        if (tok == tok_number) {
            std::cout << "Token: Number (" << NumVal << ")\n";
        } else {
            std::cout << "Token: Operator '" << (char)tok << "'\n";
        }
    }

    return 0;
}

