#!/bin/bash
set -e

if [ $# -eq 0 ]; then
    echo "Usage: $0 <input.kirk>"
    exit 1
fi

INPUT_FILE=$1

echo "Compiling Kirk source: $INPUT_FILE"
./kirk "$INPUT_FILE"

echo "Generating object file"
echo "Modern Linux defaults to PIE (Position Independent Executables) for security, thus I added the flag relocation model to prevent errors"
llc -relocation-model=pic -filetype=obj output.ll -o output.o

echo "Linking executable"
clang output.o -o program

echo "Running program"
./program

echo ""
echo "Cleaning up"
rm -f output.o
rm -f program

echo "Done!"
