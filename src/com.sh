#!/bin/bash

# Check if the CPP file is provided as a command-line argument
if [ $# -eq 0 ]; then
    echo "Error: No CPP file provided"
    exit 1
fi

# Get the CPP file path from the command-line argument
cpp_file="$1"

# Check if the CPP file exists
if [ ! -f "$cpp_file" ]; then
    echo "Error: CPP file '$cpp_file' does not exist"
    exit 1
fi

# Get the file name without extension
file_name=$(basename "$cpp_file" .cpp)

# Compile the CPP file and save the binary in the "bin" folder
g++ -I/home/phantasma/ParallelProject/lib -I/home/phantasma/ParallelProject/data -o "/home/phantasma/ParallelProject/bin/$file_name" "$cpp_file" -lssl -lcrypto

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Binary saved in bin/$file_name"
    # Execute the binary
    "/home/phantasma/ParallelProject/bin/$file_name"
else
    echo "Compilation failed"
fi
