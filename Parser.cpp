#include "Parser.h"
#include "AST.h"
#include "Lexer.h"
#include <iostream>
#include <map>

// Current state of the Parser
int CurTok; // The current token the parser is looking at
static std::map<char, int> BinopPrecedence; // Precedence table: '*' > '+'

// Reads the next token from the Lexer and updates CurTok
int getNextToken() { return CurTok = gettok(); }

void InitializePrecedence() {
  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40;
  BinopPrecedence['/'] = 40;
  BinopPrecedence['%'] = 40;
}

// Returns the priority of the current operator. If it's not an operator (like a
// number), returns -1.
static int GetTokPrecedence() {
  if (!isascii(CurTok))
    return -1;

  int TokPrec = BinopPrecedence[CurTok];
  if (TokPrec <= 0)
    return -1;

  return TokPrec;
}

std::unique_ptr<ExprAST> ParseExpression();

// Called when CurTok is a Number.
static std::unique_ptr<ExprAST> ParseNumberExpr() {
  auto Result = std::make_unique<NumberExprAST>(NumVal);
  getNextToken(); // consume the number
  return Result;
}

static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
  std::string IdName = IdentifierStr;
  getNextToken();

  if (CurTok == TOK_ASSIGN) {
    getNextToken();

    auto RHS = ParseExpression();
    if (!RHS) {
      return nullptr;
    }

    // Variable Assignmnent
    return std::make_unique<AssignmentExprAST>(IdName, std::move(RHS));
  }

  // Variable Reference
  return std::make_unique<VariableExprAST>(IdName);
}

// "Primary" means the basic building blocks: numbers or parentheses.
static std::unique_ptr<ExprAST> ParsePrimary() {
  switch (CurTok) {
  default:
    std::cerr << "Error: unknown token when expecting an expression!\n";
    return nullptr;

  // Handle Identifiers (Variables)
  case TOK_IDENTIFIER:
    return ParseIdentifierExpr();

  // Handle Numbers
  case TOK_NUMBER:
    return ParseNumberExpr();

  // Handle Parentheses
  case '(':
    getNextToken(); // eat '('
    auto V = ParseExpression();
    if (!V)
      return nullptr;

    if (CurTok != ')') {

      std::cerr << "Error: expected ')'\n";
      return nullptr;
    }

    getNextToken(); // eat ')'
    return V;
  }
}

// This function handles the "Right Hand Side" of an expression.
// ExprPrec: The precedence of the operator strictly to our left.
static std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                              std::unique_ptr<ExprAST> LHS) {
  while (true) {
    // Look at the next operator
    int TokPrec = GetTokPrecedence();

    // If this is a low-precedence operator (or not an operator at all), then we
    // are done with the current block. Return what we have. Example: If we are
    // parsing "4 * 5 + 1", and we just built "4 * 5", the next op is "+". Since
    // "+" (20) < "*" (40), we stop and return.
    if (TokPrec < ExprPrec)
      return LHS;

    int BinOp = CurTok;
    getNextToken(); // consume binop

    // Parse the primary expression after the binary operator
    auto RHS = ParsePrimary();
    if (!RHS)
      return nullptr;

    // LOOKAHEAD: Is the *next* operator even stronger?
    // Example: "a + b * c"
    // We are at "+". We parsed "b". Next op is "*". Since "*" is stronger than
    // "+", we let "*" steal "b" as its LHS.
    int NextPrec = GetTokPrecedence();
    if (TokPrec < NextPrec) {
      // Recursively parse the high-priority part first
      RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
      if (!RHS)
        return nullptr;
    }

    // Merge LHS and RHS into a new node
    LHS =
        std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
  }
}

// Entry point for parsing
std::unique_ptr<ExprAST> ParseExpression() {
  auto LHS = ParsePrimary();
  if (!LHS)
    return nullptr;

  return ParseBinOpRHS(0, std::move(LHS));
}

// Parse API
std::unique_ptr<ExprAST> Parse() {
  getNextToken();
  return ParseExpression();
}
