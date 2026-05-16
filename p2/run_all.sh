#!/bin/bash

mkdir -p build && cd build && cmake .. && cmake --build . && cd ..

mkdir -p output

for ranges_path in ranges/ranges*.txt; do
    filename=$(basename "$ranges_path")

    index="${filename//[^0-9]/}"

    registers_path="registers/registers${index}.txt"
    output_path="output/allocation${index}.txt"

    if [ ! -f "$registers_path" ]; then
        echo "  [SKIP] Missing registers file:"
        echo "         $registers_path"
        continue
    fi

    ./myProg -b \
        "$ranges_path" \
        "$registers_path" \
        "$output_path"

done