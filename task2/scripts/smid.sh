#!/bin/bash

SOURCE="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/src/matmul_simd.cpp"
PROJECT_DIR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2"
DISABLE_PREFETCH="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/disable_prefetch.sh"
RESTORE_MSR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/restore_msr.sh"

CSV="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task2/results/SMID_results.csv"

# 1. Define the parameters to sweep
MATRIX_SIZES=(256 512 752 1024 1256 1504 1752 2048)

REGISTER_WIDTHS=(128 256 512)


# =========================================================
# Restore hardware prefetcher on exit
# =========================================================

cleanup() {
    echo ""
    echo "=============================================="
    echo "Enabling / restoring hardware prefetcher"
    echo "=============================================="

    bash "$RESTORE_MSR"

    echo "Hardware prefetcher restored."
}

# Make sure restore_msr.sh runs when script exits,
# including Ctrl+C
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

echo "matrix_size,simd_register_width,instructions,l1d_misses,speedup" > "$CSV"

# =========================================================
# Run experiments
# =========================================================

for MATRIXSIZE in "${MATRIX_SIZES[@]}"; do
    for REGWIDTH in "${REGISTER_WIDTHS[@]}"; do
        
        # 2. Map the width to the correct C++ intrinsics and distance
        if [ "$REGWIDTH" -eq 128 ]; then
            PREFIX="_mm_"
            DIST=4
        elif [ "$REGWIDTH" -eq 256 ]; then
            PREFIX="_mm256_"
            DIST=8
        elif [ "$REGWIDTH" -eq 512 ]; then
            PREFIX="_mm512_"
            DIST=16
        fi

        echo -n "Building Size: ${MATRIXSIZE} | SIMD: ${REGWIDTH}-bit ... "

        # 3. Execute the sed mutations
        # Change the data type (e.g., __m256 -> __m128)
        sed -i -E "s/__m(128|256|512)/__m${REGWIDTH}/g" "$SOURCE"
        
        # Change the function prefix (e.g., _mm256_loadu_ps -> _mm_loadu_ps)
        sed -i -E "s/_mm(256|512)?_/${PREFIX}/g" "$SOURCE"
        
        # Change the dist variable
        sed -i -E "s/int dist = (4|8|16);/int dist = ${DIST};/g" "$SOURCE"

        # -------------------------------------------------
        # Build
        # -------------------------------------------------

        echo "Running make..."

        if ! make -C "$PROJECT_DIR"; then
            echo "ERROR: make failed for MATRIXSIZE=$MATRIXSIZE REGWIDTH=$REGWIDTH"
            continue
        fi


        # -------------------------------------------------
        # Run perf
        # -------------------------------------------------

        OUTPUT=$(mktemp)

        sudo perf stat \
            -e cpu_core/cycles/ \
            -e cpu_core/instructions/ \
            "$PROJECT_DIR/bin/conv" simd "$MATRIXSIZE" "$MATRIXSIZE" 9 50 \
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
        # Extract speedup
        # -------------------------------------------------

        SPEEDUP=$(grep '^simd' "$OUTPUT" \
            | awk '{gsub(/x$/, "", $5); print $5}')


        # -------------------------------------------------
        # Save to CSV
        # -------------------------------------------------

        echo "$MATRIXSIZE,$REGWIDTH,$INSTRUCTIONS,$SPEEDUP" >> "$CSV"

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

echo ""
echo "Hardware prefetcher will now be restored."