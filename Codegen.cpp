#include "Codegen.h"
#include "AST.h"
#include "Algorithms.h"
#include "Lexer.h"
#include "llvm/IR/Verifier.h"
#include <iostream>

using namespace llvm;

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<IRBuilder<>> Builder;
std::unique_ptr<Module> TheModule;

std::map<std::string, AllocaInst *> NamedValues;

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

// This creates an alloca instruction in the entry block of a function
AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                   const std::string &VarName) {
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                   TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(Type::getDoubleTy(*TheContext), 0, VarName.c_str());
}

Value *VariableExprAST::codegen() {
  // Look up the variable in the symbol table
  auto Iter = NamedValues.find(Name);
  AllocaInst *A = (Iter != NamedValues.end()) ? Iter->second : nullptr;

  if (A) {
    return Builder->CreateLoad(A->getAllocatedType(), A, Name.c_str());
  }

  std::string ErrorMsg = "Unknown variable name: '" + Name + "'";

  if (Name.length() > 2) {
    std::vector<std::string> Candidates;

    // Iterate over all defined variables
    for (const auto &pair : NamedValues) {
      const std::string &KnownVar = pair.first;

      if (KnownVar == Name)
        continue;

      // Calculate distance
      int Dist = getLevenshteinDistance(Name, KnownVar);

      // If close enough (distance <= 2), add to candidates
      if (Dist <= 2) {
        Candidates.push_back(KnownVar);
      }
    }

    // Print suggestions if we found any
    if (!Candidates.empty()) {
      ErrorMsg += ". Maybe you meant: ";
      for (size_t i = 0; i < Candidates.size(); ++i) {
        ErrorMsg += "'" + Candidates[i] + "'";
        // Add a comma if it's not the last one
        if (i != Candidates.size() - 1)
          ErrorMsg += ", ";
      }
      ErrorMsg += "?";
    }
  }

  LogErrorAt(Loc, ErrorMsg);
  exit(1);
}

llvm::Value *AssignmentExprAST::codegen() {
  // Generate code for the RHS first
  Value *Val = RHS->codegen();
  if (!Val)
    return nullptr;

  // Look up the variable
  AllocaInst *Alloca = NamedValues[Name];

  // If it doesn't exist, create it (declaration + assignment)
  if (!Alloca) {
    // Get the current function we are generating code into
    Function *TheFunction = Builder->GetInsertBlock()->getParent();

    // Create the 'alloca' for this variable
    Alloca = CreateEntryBlockAlloca(TheFunction, Name);

    // Add it to the Symbol Table so we can find it later
    NamedValues[Name] = Alloca;
  }

  // Generate the Store instruction
  Builder->CreateStore(Val, Alloca);

  // Assignment expressions usually return the value assigned (allows x = y = 5)
  return Val;
}
