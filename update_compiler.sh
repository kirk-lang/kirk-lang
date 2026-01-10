#!/bin/bash
set -e

echo "Updating the Compiler"
clang++ main.cpp Lexer.cpp Parser.cpp Codegen.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o kirk

echo "Kirk Compiler was updated. Now the new Compiler will be in effect."
