#!/bin/bash

mkdir -p build && cd build && cmake .. && cmake --build . && cd ..

mkdir -p output
mkdir -p risk

for input_file in input/*.csv; do
    base=$(basename "$input_file" .csv)
    echo "Running: $base"
    ./myProg -b "$input_file" "risk/${base}.csv"
done