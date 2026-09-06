#!/bin/bash

SOURCE="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/src/matmul_simd.cpp"
PROJECT_DIR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2"
DISABLE_PREFETCH="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/disable_prefetch.sh"
RESTORE_MSR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/restore_msr.sh"

CSV="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/results/naive_results.csv"

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
# Create CSV
# =========================================================

mkdir -p "${PROJECT_DIR}/results"
echo "matrix_size,naive_instructions,l1d_misses,l1d_loads,naive_time" > "$CSV"

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
        -e cpu_core/L1-dcache-load-misses/ \
        -e cpu_core/L1-dcache-loads/ \
        "$PROJECT_DIR/bin/matmul" naive "$MATRIXSIZE" "$MATRIXSIZE" "$MATRIXSIZE" 50 \
        > "$NAIVE_OUTPUT" 2>&1

    NAIVE_INSTRUCTIONS=$(grep 'cpu_core/instructions' "$NAIVE_OUTPUT" \
        | sed 's/,//g' \
        | awk '{print $1}')

    NAIVE_TIME=$(grep '^naive' "$NAIVE_OUTPUT" \
        | awk '{print $4}')
        
    L1D_MISSES=$(grep 'cpu_core/L1-dcache-load-misses' "$NAIVE_OUTPUT" \
        | sed 's/,//g' \
        | awk '{print $1}')

    L1D_LOADS=$(grep 'cpu_core/L1-dcache-loads' "$NAIVE_OUTPUT" \
        | sed 's/,//g' \
        | awk '{print $1}')

    # Display output
    cat "$NAIVE_OUTPUT"

    # -------------------------------------------------
    # Save to CSV
    # -------------------------------------------------

    echo "$MATRIXSIZE,$NAIVE_INSTRUCTIONS,$L1D_MISSES,$L1D_LOADS,$NAIVE_TIME" >> "$CSV"

    echo ""
    echo "Captured:"
    echo "  Naive Instructions : $NAIVE_INSTRUCTIONS"
    echo "  L1D_Misses         : $L1D_MISSES"
    echo "  L1D_Loads          : $L1D_LOADS"
    echo "  Naive Time         : $NAIVE_TIME ms"

    rm -f "$NAIVE_OUTPUT"

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