#!/bin/bash

CORRECTION="./myCorrection"

RANGES_DIR="ranges"
REGISTERS_DIR="registers"
OUTPUT_DIR="output"

pass=0
fail=0
skip=0


for ranges_path in "$RANGES_DIR"/ranges*.txt; do
    filename=$(basename "$ranges_path")

    index="${filename//[^0-9]/}"

    registers_path="$REGISTERS_DIR/registers${index}.txt"
    output_path="$OUTPUT_DIR/allocation${index}.txt"

    echo ""
    echo "--------------------------------------------"
    echo "  Dataset $index"
    echo "--------------------------------------------"

    if [ ! -f "$registers_path" ]; then
        echo "  [SKIP] Missing registers file:"
        echo "         $registers_path"
        ((skip++))
        continue
    fi

    if [ ! -f "$output_path" ]; then
        echo "  [SKIP] Missing allocation file:"
        echo "         $output_path"
        echo "         Run ./run_all.sh first."
        ((skip++))
        continue
    fi

    result=$("$CORRECTION" -c \
        "$ranges_path" \
        "$registers_path" \
        "$output_path" 2>&1)

    echo "$result" | sed 's/^/  /'

    if echo "$result" | grep -q "^FAIL"; then
        ((fail++))
    else
        ((pass++))
    fi
done

echo ""
echo "============================================"
echo "  Summary: $pass passed, $fail failed, $skip skipped"
echo "============================================"