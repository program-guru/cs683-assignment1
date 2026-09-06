#!/bin/bash

# =========================================================
# Validate Command-Line Argument
# =========================================================

if [ -z "$1" ]; then
    echo "Usage: $0 <operation>"
    echo "Example: $0 tile"
    exit 1
fi

OPERATION="$1"

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "$SCRIPT_DIR/.." && pwd)"
CSV="${PROJECT_DIR}/results/${OPERATION}_results.csv"

MATRIX_SIZES=(256 512 752 1024 1256 1504 1752 2048)

# =========================================================
# Get sudo permission once
# =========================================================

echo "Requesting sudo access..."
sudo -v || exit 1



# =========================================================
# Create CSV
# =========================================================

mkdir -p "${PROJECT_DIR}/results"
echo "matrix_size,instructions,l1d_misses,speedup" > "$CSV"


# =========================================================
# Run experiments
# =========================================================

for MATRIXSIZE in "${MATRIX_SIZES[@]}"; do

    echo ""
    echo "=============================================="
    echo "OPERATION=$OPERATION  MATRIXSIZE=$MATRIXSIZE"
    echo "=============================================="


    while true; do
        # -------------------------------------------------
        # Run perf
        # -------------------------------------------------

        OUTPUT=$(mktemp)

        sudo perf stat \
            -e cpu_core/cycles/ \
            -e cpu_core/instructions/ \
            -e cpu_core/l1d_miss.load/ \
            "$PROJECT_DIR/bin/conv" "$OPERATION" "$MATRIXSIZE" "$MATRIXSIZE" 7 50 \
            > "$OUTPUT" 2>&1


        # -------------------------------------------------
        # Display output
        # -------------------------------------------------

        cat "$OUTPUT"


        # -------------------------------------------------
        # Extract instructions
        # -------------------------------------------------

        INSTRUCTIONS=$(grep 'cpu_core/instructions' "$OUTPUT" \
            | sed 's/,//g' \
            | awk '{print $1}')


        # -------------------------------------------------
        # Extract L1-D misses
        # -------------------------------------------------

        L1D_MISSES=$(grep 'cpu_core/l1d_miss.load' "$OUTPUT" \
            | sed 's/,//g' \
            | awk '{print $1}')


        # -------------------------------------------------
        # Extract speedup
        # -------------------------------------------------

        SPEEDUP=$(grep "^${OPERATION}" "$OUTPUT" \
            | awk '{gsub(/x$/, "", $5); print $5}')


        # -------------------------------------------------
        # Save to CSV
        # -------------------------------------------------

        if [[ "$INSTRUCTIONS" =~ ^[0-9]+$ && "$L1D_MISSES" =~ ^[0-9]+$ ]]; then
            echo "$MATRIXSIZE,$INSTRUCTIONS,$L1D_MISSES,$SPEEDUP" >> "$CSV"
            rm -f "$OUTPUT"
            break
        else
            echo "Invalid instructions or L1-D misses value; rerunning matrix size $MATRIXSIZE"
            rm -f "$OUTPUT"
        fi
    done

    echo ""
    echo "Captured:"
    echo "  Instructions : $INSTRUCTIONS"
    echo "  L1-D misses  : $L1D_MISSES"
    echo "  Speedup      : $SPEEDUP"

done


# =========================================================
# Finished
# =========================================================

echo ""
echo "=============================================="
echo "All experiments complete for operation: $OPERATION"
echo "=============================================="

echo ""
echo "Results saved to $CSV:"
cat "$CSV"