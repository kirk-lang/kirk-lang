#include <iostream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

int main() {
  LLVMContext TheContext;

  Module* KirkModule = new Module("Kirk Compiler", TheContext);

  KirkModule->print(outs(), nullptr);

  std::cout << "\n[Success] LLVM is working!\n";
  
  return 0;;
}

