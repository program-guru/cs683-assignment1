#!/bin/bash

# Updated SOURCE to target matmul_prefetch.cpp
SOURCE="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/src/matmul_prefetch.cpp"
PROJECT_DIR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2"
DISABLE_PREFETCH="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/disable_prefetch.sh"
RESTORE_MSR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/restore_msr.sh"

# Updated CSV output name
CSV="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/results/prefetch_results.csv"

# 1. Define the parameters to sweep
MATRIX_SIZES=(256 512 752 1024 1256 1504 1752 2048)

# Define prefetch distances to test (added 0, 64, 128, 256, 512, 1024)
PREFETCH_DISTANCES=(16 32 64 128 256 512 1024)


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

# Updated CSV headers
echo "matrix_size,prefetch_distance,instructions,speedup" > "$CSV"

# =========================================================
# Run experiments
# =========================================================

for MATRIXSIZE in "${MATRIX_SIZES[@]}"; do
    for DIST in "${PREFETCH_DISTANCES[@]}"; do
        
        # Reset source file to clean template before applying mutations
        git checkout -- "$SOURCE"

        echo -n "Building Size: ${MATRIXSIZE} | Prefetch Distance: ${DIST} ... "

        # 3. Execute the sed mutation to change prefetch distance
        # This looks for "const int p_dist = <any number>;" and replaces it with the current loop DIST
        sed -i -E "s/const int p_dist = [0-9]+;/const int p_dist = ${DIST};/g" "$SOURCE"

        # -------------------------------------------------
        # Build
        # -------------------------------------------------

        echo "Running make..."

        if ! make -C "$PROJECT_DIR"; then
            echo "ERROR: make failed for MATRIXSIZE=$MATRIXSIZE DIST=$DIST"
            continue
        fi


        # -------------------------------------------------
        # Run perf (Changed execution target from 'simd' to 'prefetch')
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
        # Extract instructions
        # -------------------------------------------------

        INSTRUCTIONS=$(grep 'cpu_core/instructions' "$OUTPUT" \
            | sed 's/,//g' \
            | awk '{print $1}')


        # -------------------------------------------------
        # Extract speedup (Updated grep to look for 'prefetch' row)
        # -------------------------------------------------

        SPEEDUP=$(grep '^prefetch' "$OUTPUT" \
            | awk '{gsub(/x$/, "", $5); print $5}')


        # -------------------------------------------------
        # Save to CSV
        # -------------------------------------------------

        echo "$MATRIXSIZE,$DIST,$INSTRUCTIONS,$SPEEDUP" >> "$CSV"

        echo ""
        echo "Captured:"
        echo "  Instructions : $INSTRUCTIONS"
        echo "  Speedup      : $SPEEDUP"

        rm -f "$OUTPUT"

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