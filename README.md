# Kirk

Kirk is an experimental programming language and compiler written in C++. The goal is simple: fast compilation with helpful, human-friendly error messages. Instead of plain errors, Kirk explains what went wrong and suggests fixes.

- The project is in early development.
- Current focus is the frontend: lexer, parser, and diagnostics.
- LLVM is used for code generation.

## Build

Requires LLVM and a C++ compiler.

```bash
clang++ main.cpp Lexer.cpp Parser.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o kirk
```

## Run

```bash
./kirk file.kirk
```

## Status

Work in progress. Expect breaking changes.

> After a certain phase of development, both the 'Build' and 'Run' phase will be packaged in a separate "Kirk Compiler" package.

