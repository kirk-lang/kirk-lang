#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include <memory>

// We will be using operator precedence parsing here.
std::unique_ptr<ExprAST> Parse();

#endif
