#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd -- "$SCRIPT_DIR/.." && pwd)"
SOURCE="$PROJECT_DIR/src/conv_optimized.cpp"
CSV="$PROJECT_DIR/results/optimized_results.csv"

MATRIX_SIZES=(1024 1256 1504 1752 2048 2560 3072 3584 4096)
KERNEL_SIZES=(3 5 7)
IMPLEMENTATIONS=(1 2 3 4 5)


# =========================================================
# Get sudo permission once
# =========================================================

echo "Requesting sudo access..."
sudo -v || exit 1


# =========================================================
# Create CSV
# =========================================================

echo "matrix_size,kernel_size,implementation,instructions,l1d_misses,l1d_loads,speedup" > "$CSV"


# =========================================================
# Run experiments
# =========================================================

for IMPLEMENTATION in "${IMPLEMENTATIONS[@]}"; do

    echo ""
    echo "=============================================="
    echo "IMPLEMENTATION=$IMPLEMENTATION"
    echo "=============================================="

    # Select the implementation used by conv_optimized.
    sed -i -E \
        "s/implementation[1-5]\(in, out, ker, H, W, K\);/implementation${IMPLEMENTATION}(in, out, ker, H, W, K);/" \
        "$SOURCE"

    echo "Implementation changed to $IMPLEMENTATION"

    # -------------------------------------------------
    # Build
    # -------------------------------------------------

    echo "Running make..."

    if ! make -C "$PROJECT_DIR"; then
        echo "ERROR: make failed for IMPLEMENTATION=$IMPLEMENTATION"
        continue
    fi

    for KERNEL_SIZE in "${KERNEL_SIZES[@]}"; do

        for MATRIXSIZE in "${MATRIX_SIZES[@]}"; do

            echo ""
            echo "========================================================"
            echo "IMPLEMENTATION=$IMPLEMENTATION  KERNEL_SIZE=$KERNEL_SIZE  MATRIXSIZE=$MATRIXSIZE"
            echo "========================================================"


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
                    "$PROJECT_DIR/bin/conv" optimized "$MATRIXSIZE" "$MATRIXSIZE" "$KERNEL_SIZE" 50 \
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

                SPEEDUP=$(grep '^optimized' "$OUTPUT" \
                    | awk '{gsub(/x$/, "", $5); print $5}')


            # -------------------------------------------------
            # Save to CSV
            # -------------------------------------------------

                if [[ "$INSTRUCTIONS" =~ ^[0-9]+$ && "$L1D_MISSES" =~ ^[0-9]+$ && "$L1D_LOADS" =~ ^[0-9]+$ ]]; then
                    echo "$MATRIXSIZE,$KERNEL_SIZE,$IMPLEMENTATION,$INSTRUCTIONS,$L1D_MISSES,$L1D_LOADS,$SPEEDUP" >> "$CSV"
                    rm -f "$OUTPUT"
                    break
                else
                    echo "Invalid instructions, L1-D misses, or L1-D loads value; rerunning matrix size $MATRIXSIZE, kernel size $KERNEL_SIZE, implementation $IMPLEMENTATION"
                    rm -f "$OUTPUT"
                fi
            done

            echo ""
            echo "Captured:"
            echo "  Instructions : $INSTRUCTIONS"
            echo "  L1-D misses  : $L1D_MISSES"
            echo "  L1-D loads   : $L1D_LOADS"
            echo "  Speedup      : $SPEEDUP"

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