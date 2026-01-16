#include "Codegen.h"
#include "AST.h"
#include "Algorithms.h"
#include "Errors.h"
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
  case '<':
    L = Builder->CreateFCmpOLT(L, R);
    return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
  case '>':
    L = Builder->CreateFCmpOGT(L, R);
    return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
  case TOK_EQ:
    L = Builder->CreateFCmpOEQ(L, R);
    return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
  case TOK_NEQ:
    L = Builder->CreateFCmpONE(L, R);
    return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");

  default:
    SyntaxError(CurLoc, "Error: invalid binary operator");
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

  ReferenceError(Loc, Name, NamedValues).raise();
  return nullptr;
}

Value *AssignmentExprAST::codegen() {
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

Value *IfExprAST::codegen() {
  Value *CondV = Cond->codegen();
  if (!CondV)
    return nullptr;

  // Convert condition to a boolean
  CondV = Builder->CreateFCmpONE(
      CondV, ConstantFP::get(*TheContext, APFloat(0.0)), "ifcond");

  // Get the current function so we can insert blocks into it
  Function *TheFunction = Builder->GetInsertBlock()->getParent();

  // Create blocks for 'then', 'else', and 'merge'
  BasicBlock *ThenBB = BasicBlock::Create(*TheContext, "then", TheFunction);
  BasicBlock *ElseBB = BasicBlock::Create(*TheContext, "else");
  BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "ifcont");

  // Create the Conditional Branch
  // "If CondV is true, go to ThenBB, otherwise go to ElseBB"
  Builder->CreateCondBr(CondV, ThenBB, ElseBB);

  Builder->SetInsertPoint(ThenBB);

  Value *ThenV = Then->codegen();
  if (!ThenV)
    return nullptr;

  Builder->CreateBr(MergeBB);
  ThenBB = Builder->GetInsertBlock();

  TheFunction->insert(TheFunction->end(), ElseBB);
  Builder->SetInsertPoint(ElseBB);

  Value *ElseV = Else->codegen();
  if (!ElseV)
    return nullptr;

  Builder->CreateBr(MergeBB);
  ElseBB = Builder->GetInsertBlock();

  TheFunction->insert(TheFunction->end(), MergeBB);
  Builder->SetInsertPoint(MergeBB);

  // The PHI Node
  // Since 'if' is an expression, it must return a value.
  // The PHI node says: "If we came from ThenBB, use ThenV. If from ElseBB, use
  // ElseV."
  PHINode *PN = Builder->CreatePHI(Type::getDoubleTy(*TheContext), 2, "iftmp");

  PN->addIncoming(ThenV, ThenBB);
  PN->addIncoming(ElseV, ElseBB);

  return PN;
}

Value *UnaryExprAST::codegen() {
  Value *OperandV = Operand->codegen();
  if (!OperandV)
    return nullptr;

  switch (Opcode) {
  case '-':
    return Builder->CreateFNeg(OperandV);
  default:
    SyntaxError(CurLoc, "Unknown unary operator");
    return nullptr;
  }
}

Value *BlockExprAST::codegen() {
  Value *LastVal = nullptr;
  for (auto &Expr : Expressions) {
    LastVal = Expr->codegen();
  }
  return LastVal;
}

Value *PrintExprAST::codegen() {
  Value *Val = Expr->codegen();
  if (!Val)
    return nullptr;

  Function *PrintfFunc = TheModule->getFunction("printf");
  Value *FormatStr = Builder->CreateGlobalString("%.2f\n", "printstr");

  return Builder->CreateCall(PrintfFunc, {FormatStr, Val}, "printcall");
}

Value *WhileExprAST::codegen() {
  Function *TheFunction = Builder->GetInsertBlock()->getParent();

  BasicBlock *LoopCondBB =
      BasicBlock::Create(*TheContext, "loopcond", TheFunction);
  BasicBlock *LoopBodyBB = BasicBlock::Create(*TheContext, "loopbody");
  BasicBlock *AfterBB = BasicBlock::Create(*TheContext, "afterloop");

  Builder->CreateBr(LoopCondBB);
  Builder->SetInsertPoint(LoopCondBB);

  Value *CondV = Cond->codegen();
  if (!CondV)
    return nullptr;

  CondV = Builder->CreateFCmpONE(
      CondV, ConstantFP::get(*TheContext, APFloat(0.0)), "loopcond");

  // Conditional Branch: if true -> Body, else -> After
  Builder->CreateCondBr(CondV, LoopBodyBB, AfterBB);

  // Loop Body Block
  TheFunction->insert(TheFunction->end(), LoopBodyBB);
  Builder->SetInsertPoint(LoopBodyBB);

  if (!Body->codegen())
    return nullptr;

  // Jump back to the condition to loop again
  Builder->CreateBr(LoopCondBB);

  // After Loop Block
  TheFunction->insert(TheFunction->end(), AfterBB);
  Builder->SetInsertPoint(AfterBB);

  // While loops always return 0.0
  return Constant::getNullValue(Type::getDoubleTy(*TheContext));
}
