#include "Codegen.h"
#include "AST.h"
#include "llvm/IR/Verifier.h"
#include <iostream>

using namespace llvm;

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<IRBuilder<>> Builder;
std::unique_ptr<Module> TheModule;

void InitializeModule() {
  // Holds types and constants
  TheContext = std::make_unique<LLVMContext>();

  // Holds functions
  TheModule = std::make_unique<Module>("Kirk Compiler", *TheContext);

  // Builder to insert instructions
  Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

// Turns a number to LLVM Number constant
Value *NumberExprAST::codegen() {
  // APFloat is how LLVM represents floating point numbers internally
  return ConstantFP::get(*TheContext, APFloat(Val));
}

// Turns any expression into IR operation
Value *BinaryExprAST::codegen() {
  // Generate codes for the Left side and Right side
  Value *L = LHS->codegen();
  Value *R = RHS->codegen();

  if (!L || !R)
    return nullptr;

  // Create the instruction based on the operator
  switch (Op) {
  case '+':
    return Builder->CreateFAdd(L, R);
  case '-':
    return Builder->CreateFSub(L, R);
  case '*':
    return Builder->CreateFMul(L, R);
  case '/':
    return Builder->CreateFDiv(L, R);
  case '%':
    return Builder->CreateFRem(L, R);
  default:
    std::cerr << "Error: invalid binary operator" << std::endl;
    return nullptr;
  }
}
