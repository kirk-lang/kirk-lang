#include "Codegen.h"
#include "Lexer.h"
#include "Parser.h"
#include "llvm/Support/raw_ostream.h"
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

  InitializeModule(); // Initialize LLVM
  auto AST = Parse();

  if (AST) {
    std::cerr << "AST parsed successfully\n";

    FunctionType *FT = FunctionType::get(Type::getInt32Ty(*TheContext), false);
    Function *TheFunction = Function::Create(FT, Function::ExternalLinkage,
                                             "main", TheModule.get());

    // Creating code
    BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
    Builder->SetInsertPoint(BB);

    // IR generation
    Value *Result = AST->codegen();

    if (Result) {
      Value *IntResult =
          Builder->CreateFPToSI(Result, Type::getInt32Ty(*TheContext));
      Builder->CreateRet(IntResult);

      std::error_code EC;
      raw_fd_ostream OutFile("output.ll", EC);
      if (!EC) {
        TheModule->print(OutFile, nullptr);
      }
    } else {
      std::cerr << "Error: codegen() returned nullptr\n";
    }
  } else {
    std::cerr << "Error parsing file.\n";
  }

  return 0;
}
