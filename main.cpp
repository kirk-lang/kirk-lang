#include "Codegen.h"
#include "Lexer.h"
#include "Parser.h"
#include "llvm/Support/FileSystem.h"
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
    // To create a dummy function signature: "void main()" (a boiler-plate LLVM
    // to insert code)
    FunctionType *FT = FunctionType::get(Type::getVoidTy(*TheContext), false);
    Function *TheFunction = Function::Create(FT, Function::ExternalLinkage,
                                             "main", TheModule.get());

    // Create a "Basic Block" (a chunk of code)
    BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
    Builder->SetInsertPoint(BB);

    // Generate the IR and return void for the function
    AST->codegen();
    Builder->CreateRetVoid();

    std::cout << "IR Generated" << std::endl;
    TheModule->print(outs(), nullptr);
  } else {
    std::cerr << "Error parsing file.\n";
  }

  return 0;
}
