#!/bin/bash

# Build
mkdir -p build && cd build && cmake .. && cmake --build . && cd ..

# Output folder
mkdir -p output

echo "Running register allocation tests..."

# Array of (range, register, output)
tests=(
    "ranges1.txt registers2.txt allocation1.txt"
    "ranges2.txt registers2.txt allocation2.txt"
    "ranges3.txt registers2.txt allocation3.txt"
    "ranges4.txt registers1.txt allocation4.txt"
    "ranges5.txt registers1.txt allocation5.txt"
    "ranges6.txt registers3.txt allocation6.txt"
    
)

# Run all tests
for test in "${tests[@]}"; do
    set -- $test

    range_file=$1
    register_file=$2
    output_file=$3

    echo "Running $range_file with $register_file..."

    ./myProg -b "ranges/$range_file" "registers/$register_file" "output/$output_file"
done

echo "All tests completed."
