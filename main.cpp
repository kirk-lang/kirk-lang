#include "Codegen.h"
#include "Lexer.h"
#include "Parser.h"
#include "llvm/IR/Verifier.h"
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
    FunctionType *PrintfType =
        FunctionType::get(Type::getInt32Ty(*TheContext),
                          {PointerType::getUnqual(*TheContext)}, true);
    Function *PrintfFunc = Function::Create(
        PrintfType, Function::ExternalLinkage, "printf", TheModule.get());

    FunctionType *FT = FunctionType::get(Type::getInt32Ty(*TheContext), false);
    Function *TheFunction = Function::Create(FT, Function::ExternalLinkage,
                                             "main", TheModule.get());

    // Creating code
    BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
    Builder->SetInsertPoint(BB);

    // IR generation
    Value *Result = AST->codegen();

    if (Result) {
      Value *FormatStr = Builder->CreateGlobalString("%.2f\n");
      Builder->CreateCall(PrintfFunc, {FormatStr, Result});
      Builder->CreateRet(ConstantInt::get(*TheContext, APInt(32, 0)));
      verifyFunction(*TheFunction);

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
