#!/bin/bash

SOURCE="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/src/conv_simd.cpp"
PROJECT_DIR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1"
DISABLE_PREFETCH="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/disable_prefetch.sh"
RESTORE_MSR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/restore_msr.sh"

CSV="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/results/simd_results.csv"

MATRIX_SIZES=(256 512 752 1024 1256 1504 1752 2048)

# SIMD register widths to sweep, mapped to the is_simd_256 boolean
# 128 -> false (SSE), 256 -> true (AVX2)
REGISTER_WIDTHS=(128 256)


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

echo "matrix_size,simd_register_width,instructions,speedup" > "$CSV"


# =========================================================
# Run experiments
# =========================================================

for MATRIXSIZE in "${MATRIX_SIZES[@]}"; do

    for REGWIDTH in "${REGISTER_WIDTHS[@]}"; do

        echo ""
        echo "=============================================="
        echo "MATRIXSIZE=$MATRIXSIZE  SIMD_REGISTER_WIDTH=$REGWIDTH"
        echo "=============================================="

        # -------------------------------------------------
        # Change SIMD register width (is_simd_256 flag)
        # -------------------------------------------------

        if [ "$REGWIDTH" -eq 256 ]; then
            IS_SIMD_256="true"
        else
            IS_SIMD_256="false"
        fi

        sed -i \
            "s/const bool is_simd_256 = [a-z]*;  \\/\\/ Set to true to use AVX2 (256-bit) intrinsics, false for SSE (128-bit)/const bool is_simd_256 = ${IS_SIMD_256};  \\/\\/ Set to true to use AVX2 (256-bit) intrinsics, false for SSE (128-bit)/" \
            "$SOURCE"

        echo "SIMD register width changed to $REGWIDTH-bit (is_simd_256=$IS_SIMD_256)"


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