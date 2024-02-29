#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Error: No CPP file provided"
    exit 1
fi

cpp_file="$1"

if [ ! -f "$cpp_file" ]; then
    echo "Error: CPP file '$cpp_file' does not exist"
    exit 1
fi

file_name=$(basename "$cpp_file" .cpp)

g++ -I/home/phantasma/ParallelProject/lib -I/home/phantasma/ParallelProject/data -o "/home/phantasma/ParallelProject/bin/$file_name" "$cpp_file" -lssl -lcrypto

if [ $? -eq 0 ]; then
    echo "Compilation successful. Binary saved in bin/$file_name"
    "/home/phantasma/ParallelProject/bin/$file_name"
else
    echo "Compilation failed"
fi
