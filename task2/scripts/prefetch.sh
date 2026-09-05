#!/bin/bash

# Updated SOURCE to target matmul_prefetch.cpp
SOURCE="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/src/matmul_prefetch.cpp"
PROJECT_DIR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2"
DISABLE_PREFETCH="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/disable_prefetch.sh"
RESTORE_MSR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/restore_msr.sh"

CSV="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/results/prefetch_results.csv"

# 1. Define the parameters to sweep
MATRIX_SIZES=(256 512 752 1024 1256 1504)
PREFETCH_DISTANCES=(16 32 64 128 256 512)
CACHE_LEVELS=(0 1 2 3)

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
# ADDED cache_level TO HEADER
echo "matrix_size,prefetch_distance,cache_level,instructions,speedup" > "$CSV"

# =========================================================
# Run experiments
# =========================================================
for MATRIXSIZE in "${MATRIX_SIZES[@]}"; do
    for DIST in "${PREFETCH_DISTANCES[@]}"; do
        for C_LEVEL in "${CACHE_LEVELS[@]}"; do # NEW LOOP FOR CACHE LEVEL
            
            # Reset source file to clean template before applying mutations
            git checkout -- "$SOURCE"

            echo -n "Building Size: ${MATRIXSIZE} | Dist: ${DIST} | Cache Level: ${C_LEVEL} ... "

            # 3. Execute the sed mutations
            sed -i -E "s/const int p_dist = [0-9]+;/const int p_dist = ${DIST};/g" "$SOURCE"
            sed -i -E "s/const int cache_level = [0-9]+;/const int cache_level = ${C_LEVEL};/g" "$SOURCE"

            # -------------------------------------------------
            # Build
            # -------------------------------------------------
            echo "Running make..."
            if ! make -C "$PROJECT_DIR"; then
                echo "ERROR: make failed for MATRIXSIZE=$MATRIXSIZE DIST=$DIST C_LEVEL=$C_LEVEL"
                continue
            fi

            # -------------------------------------------------
            # Run perf
            # -------------------------------------------------
            OUTPUT=$(mktemp)
            sudo perf stat \
                -e cpu_core/cycles/ \
                -e cpu_core/instructions/ \
                "$PROJECT_DIR/bin/matmul" prefetch "$MATRIXSIZE" "$MATRIXSIZE" "$MATRIXSIZE" 50 \
                > "$OUTPUT" 2>&1

            # -------------------------------------------------
            # Display output
            # -------------------------------------------------
            cat "$OUTPUT"

            # -------------------------------------------------
            # Extract metrics
            # -------------------------------------------------
            INSTRUCTIONS=$(grep 'cpu_core/instructions' "$OUTPUT" | sed 's/,//g' | awk '{print $1}')
            SPEEDUP=$(grep '^prefetch' "$OUTPUT" | awk '{gsub(/x$/, "", $5); print $5}')
            L1D_MISSES=$(grep 'cpu_core/l1d_miss.load' "$OUTPUT" \
            | sed 's/,//g' \
            | awk '{print $1}')

            

            # -------------------------------------------------
            # Save to CSV
            # -------------------------------------------------
            echo "$MATRIXSIZE,$DIST,$C_LEVEL,$INSTRUCTIONS,$L1D_MISSES,$SPEEDUP" >> "$CSV"

            echo ""
            echo "Captured:"
            echo "  Instructions : $INSTRUCTIONS"
            echo "  L1D_Misses   : $L1D_MISSES"
            echo "  Speedup      : $SPEEDUP"

            rm -f "$OUTPUT"
        done
    done
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