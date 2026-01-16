# Kirk

<a href="https://app.devdoq.com/dashboard/1127649459?name=kirk-lang&tab=document"><img src="https://app.devdoq.com/shields/read_docs.png" alt="Read Docs" width="120" height="45"></a> <a href="https://app.devdoq.com/dashboard/1127649459?name=kirk-lang&tab=chat"><img src="https://app.devdoq.com/shields/talk_to_code.png" alt="Talk to Code" width="165" height="60"></a>

Kirk is an experimental programming language and compiler written in C++. The goal is simple: fast compilation with helpful, human-friendly error messages. Instead of plain errors, Kirk explains what went wrong and suggests fixes.

- The project is in early development.
- Current focus is the frontend: lexer, parser, and diagnostics.
- LLVM is used for code generation.

## Features (Implemented)

* **Variables:** Support for variable declaration, assignment, and lookups (`width = 10`).
* **Math:** Full support for arithmetic operators (`+`, `-`, `*`, `/`, `%`) with operator precedence.
* **Memory Management:** Automatic stack allocation using LLVM `alloca`, `store`, and `load`.
* **LLVM Backend:** Compiles source code directly to optimized LLVM IR (`output.ll`).
* **Smart Compiler (Phase 1):** Support for smart compiler error enhancements, where it suggests you what changes to make.

## Build and Run

Requires LLVM and a C++ compiler.

### Quick Start

Use the included helper script to compile the compiler, generate the IR, link it, and run the executable in one go:

```bash
chmod +x update_compiler.sh
./update_compiler.sh
chmod +x compile_and_run.sh
./compile_and_run.sh test.kirk
```

### Manual Build

If you want to build the compiler binary manually:

```bash
clang++ main.cpp Lexer.cpp Parser.cpp Codegen.cpp Algorithms.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o kirk
```

Rest steps will be the same from the Quick Start section.

## Example Code 

```kirk
width = 10
height = 5

# Variables are mutable and memory-managed
area = width * height
width = 20
new_area = width * height

# Complex expressions with precedence
result = (width + height) * 2
```

## Roadmap

[x] Basic Arithmetic
[x] Variables & Memory Assignment
[x] Better Error Diagnostics
[x] Control Flow (if / else)
[x] Comparison Operators (<, >, ==)
[ ] Functions

## Status

**Active Development.**

The compiler currently functions as a powerful programmable calculator with variable state.

> After a certain phase of development, both the 'Build' and 'Run' phase will be packaged in a separate "Kirk Compiler" package.

