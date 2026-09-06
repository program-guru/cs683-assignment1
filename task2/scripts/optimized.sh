#!/bin/bash

SOURCE="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/src/matmul_optimized.cpp"
PROJECT_DIR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2"
DISABLE_PREFETCH="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/disable_prefetch.sh"
RESTORE_MSR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/restore_msr.sh"

CSV="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/results/optimized_results.csv"

# 1. Define the parameters to sweep
MATRIX_SIZES=(256 512 752 1024 1256 1504 1752 2048)


# =========================================================
# Restore hardware prefetcher and source file on exit
# =========================================================

cleanup() {
    echo ""
    echo "=============================================="
    echo "Enabling / restoring hardware prefetcher"
    echo "=============================================="

    bash "$RESTORE_MSR"
    git checkout -- "$SOURCE"

    echo "Hardware prefetcher and source file restored."
}

# Make sure cleanup runs when script exits, including Ctrl+C
trap cleanup EXIT


# =========================================================
# Get sudo permission once
# =========================================================

echo "Requesting sudo access..."
sudo -v || exit 1


# =========================================================
# Disable hardware prefetcher
# =========================================================

echo ""
echo "=============================================="
echo "Disabling hardware prefetcher"
echo "=============================================="

bash "$DISABLE_PREFETCH"

if [ $? -ne 0 ]; then
    echo "ERROR: Failed to disable hardware prefetcher."
    exit 1
fi

echo "Hardware prefetcher disabled."


# =========================================================
# Create CSV (Added naive_instructions and simd_instructions)
# =========================================================

mkdir -p "${PROJECT_DIR}/results"
echo "matrix_size,naive_instructions,optimized_instructions,naive_time,optimized_time,l1D_misses,speedup" > "$CSV"

# =========================================================
# Run experiments
# =========================================================

for MATRIXSIZE in "${MATRIX_SIZES[@]}"; do
        
    # Reset source file to clean template before applying mutations
    git checkout -- "$SOURCE"



    echo -n "Building Size: ${MATRIXSIZE} ... "


    # -------------------------------------------------
    # Build
    # -------------------------------------------------

    echo "Running make..."

    if ! make -C "$PROJECT_DIR"; then
        echo "ERROR: make failed for MATRIXSIZE=$MATRIXSIZE"
        continue
    fi


    # -------------------------------------------------
    # Run perf for NAIVE implementation
    # -------------------------------------------------

    NAIVE_OUTPUT=$(mktemp)

    sudo perf stat \
        -e cpu_core/cycles/ \
        -e cpu_core/instructions/ \
        "$PROJECT_DIR/bin/matmul" naive "$MATRIXSIZE" "$MATRIXSIZE" "$MATRIXSIZE" 50 \
        > "$NAIVE_OUTPUT" 2>&1

    NAIVE_INSTRUCTIONS=$(grep 'cpu_core/instructions' "$NAIVE_OUTPUT" \
        | sed 's/,//g' \
        | awk '{print $1}')

    NAIVE_TIME=$(grep '^naive' "$NAIVE_OUTPUT" \
        | awk '{print $4}')


    # -------------------------------------------------
    # Run perf for SIMD implementation
    # -------------------------------------------------

    OPTIMIZED_OUTPUT=$(mktemp)

    sudo perf stat \
        -e cpu_core/cycles/ \
        -e cpu_core/instructions/ \
        -e cpu_core/l1d_miss.load/ \
        "$PROJECT_DIR/bin/matmul" optimized "$MATRIXSIZE" "$MATRIXSIZE" "$MATRIXSIZE" 50 \
        > "$OPTIMIZED_OUTPUT" 2>&1

    # Display OPTIMIZED output
    cat "$OPTIMIZED_OUTPUT"

    OPTIMIZED_INSTRUCTIONS=$(grep 'cpu_core/instructions' "$OPTIMIZED_OUTPUT" \
        | sed 's/,//g' \
        | awk '{print $1}')

    OPTIMIZED_TIME=$(grep '^optimized' "$OPTIMIZED_OUTPUT" \
        | awk '{print $3}')

    SPEEDUP=$(grep '^optimized' "$OPTIMIZED_OUTPUT" \
        | awk '{gsub(/x$/, "", $5); print $5}')
    L1D_MISSES=$(grep 'cpu_core/l1d_miss.load' "$OUTPUT" \
            | sed 's/,//g' \
            | awk '{print $1}')


    # -------------------------------------------------
    # Save to CSV
    # -------------------------------------------------

    echo "$MATRIXSIZE,$NAIVE_INSTRUCTIONS,$OPTIMIZED_INSTRUCTIONS,$NAIVE_TIME,$OPTIMIZED_TIME,$L1D_MISSES,$SPEEDUP" >> "$CSV"

    echo ""
    echo "Captured:"
    echo "  Naive Instructions    : $NAIVE_INSTRUCTIONS"
    echo "  OPTIMIZED Instructions: $OPTIMIZED_INSTRUCTIONS"
    echo "  Naive Time            : $NAIVE_TIME ms"
    echo "  OPTIMIZED Time        : $OPTIMIZED_TIME ms"
    echo "  Speedup               : $SPEEDUP"

    rm -f "$NAIVE_OUTPUT" "$OPTIMIZED_OUTPUT"

done


# =========================================================
# Finished
# =========================================================

echo ""
echo "=============================================="
echo "All experiments complete"
echo "=============================================="

echo ""
echo "Results:"
cat "$CSV"