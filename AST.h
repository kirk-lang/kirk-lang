#ifndef AST_H
#define AST_H

#include "llvm/IR/Value.h"
#include <memory>

// Base Expressions Class: Everything, "5", "5 + 10", etc. are expressions
class ExprAST {
public:
  virtual ~ExprAST() = default;

  virtual llvm::Value *codegen() = 0;
};

// Number Node (Leaf)
class NumberExprAST : public ExprAST {
  double Val; // Actual value, like "5", "5.0", etc.
public:
  NumberExprAST(double Val) : Val(Val) {}

  llvm::Value *codegen() override;
};

// Binary Operation Node (Branch): Holds the Operator ('+'), the Left side (A),
// and the Right side (B).
class BinaryExprAST : public ExprAST {
  char Op; // The operator, like '+', '-', etc.
  std::unique_ptr<ExprAST> LHS;
  std::unique_ptr<ExprAST> RHS;

public:
  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  llvm::Value *codegen() override;
};

#endif
