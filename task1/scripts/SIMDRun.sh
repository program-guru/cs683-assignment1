#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "$SCRIPT_DIR/.." && pwd)"
SOURCE="$PROJECT_DIR/src/conv_simd.cpp"
CSV="$PROJECT_DIR/results/simd_results.csv"

MATRIX_SIZES=(1024 1256 1504 1752 2048 2560 3072 3584 4096)

# SIMD register widths to sweep, mapped to the is_simd_256 boolean
# 128 -> false (SSE), 256 -> true (AVX2)
REGISTER_WIDTHS=(128 256)


# =========================================================
# Get sudo permission once
# =========================================================

echo "Requesting sudo access..."
sudo -v || exit 1


# =========================================================
# Create CSV
# =========================================================

echo "matrix_size,simd_register_width,instructions,l1d_misses,l1d_loads,speedup" > "$CSV"


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


        while true; do
            # -------------------------------------------------
            # Run perf
            # -------------------------------------------------

            OUTPUT=$(mktemp)

            sudo perf stat \
                -e cpu_core/cycles/ \
                -e cpu_core/instructions/ \
                -e cpu_core/L1-dcache-load-misses/ \
                -e cpu_core/L1-dcache-loads/ \
                "$PROJECT_DIR/bin/conv" simd "$MATRIXSIZE" "$MATRIXSIZE" 7 50 \
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

            L1D_MISSES=$(grep 'cpu_core/L1-dcache-load-misses' "$OUTPUT" \
                | sed 's/,//g' \
                | awk '{print $1}')

            L1D_LOADS=$(grep 'cpu_core/L1-dcache-loads' "$OUTPUT" \
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

            if [[ "$INSTRUCTIONS" =~ ^[0-9]+$ && "$L1D_MISSES" =~ ^[0-9]+$ && "$L1D_LOADS" =~ ^[0-9]+$ ]]; then
                echo "$MATRIXSIZE,$REGWIDTH,$INSTRUCTIONS,$L1D_MISSES,$L1D_LOADS,$SPEEDUP" >> "$CSV"
                rm -f "$OUTPUT"
                break
            else
                echo "Invalid instructions, L1-D misses, or L1-D loads value; rerunning matrix size $MATRIXSIZE, register width $REGWIDTH"
                rm -f "$OUTPUT"
            fi
        done

        echo ""
        echo "Captured:"
        echo "  Instructions : $INSTRUCTIONS"
        echo "  L1-D Misses  : $L1D_MISSES"
        echo "  L1-D Loads   : $L1D_LOADS"
        echo "  Speedup      : $SPEEDUP"

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