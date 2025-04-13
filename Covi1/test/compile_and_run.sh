#!/bin/bash

# Check if the source file is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 path/to/source.covi"
    exit 1
fi

SOURCE_FILE=$1
OUTPUT_FILE="${SOURCE_FILE%.covi}.fac"

# Compile the source file
./compiler/covicc "$SOURCE_FILE" -o "$OUTPUT_FILE"
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# Run the compiled bytecode
./vm/covim "$OUTPUT_FILE"
if [ $? -ne 0 ]; then
    echo "Execution failed."
    exit 1
fi

echo "Compilation and execution completed successfully."