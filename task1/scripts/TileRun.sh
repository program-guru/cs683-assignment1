#!/bin/bash

SOURCE="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/src/conv_tile.cpp"
PROJECT_DIR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1"
DISABLE_PREFETCH="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/disable_prefetch.sh"
RESTORE_MSR="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/MSR Handling/restore_msr.sh"

CSV="/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/results/tile_results.csv"

MATRIX_SIZES=(256 512 752 1024 1256 1504 1752 2048)
TILE_SIZES=(8 16 32 64 128 256 512 1024)


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

echo "matrix_size,tile_size,instructions,l1d_misses,speedup" > "$CSV"


# =========================================================
# Run experiments
# =========================================================

for MATRIXSIZE in "${MATRIX_SIZES[@]}"; do

    for TILESIZE in "${TILE_SIZES[@]}"; do

        echo ""
        echo "=============================================="
        echo "MATRIXSIZE=$MATRIXSIZE  TILESIZE=$TILESIZE"
        echo "=============================================="

        # -------------------------------------------------
        # Change tile size
        # -------------------------------------------------

        sed -i \
            "s/const int tile_size = [0-9]*;  \\/\\/ tile size for cache tiling/const int tile_size = ${TILESIZE};  \\/\\/ tile size for cache tiling/" \
            "$SOURCE"

        echo "Tile size changed to $TILESIZE"


        # -------------------------------------------------
        # Build
        # -------------------------------------------------

        echo "Running make..."

        if ! make -C "$PROJECT_DIR"; then
            echo "ERROR: make failed for MATRIXSIZE=$MATRIXSIZE TILESIZE=$TILESIZE"
            continue
        fi


        # -------------------------------------------------
        # Run perf
        # -------------------------------------------------

        OUTPUT=$(mktemp)

        sudo perf stat \
            -e cpu_core/cycles/ \
            -e cpu_core/instructions/ \
            -e cpu_core/l1d_miss.load/ \
            "$PROJECT_DIR/bin/conv" tile "$MATRIXSIZE" "$MATRIXSIZE" 9 50 \
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

        SPEEDUP=$(grep '^tile' "$OUTPUT" \
            | awk '{gsub(/x$/, "", $5); print $5}')


        # -------------------------------------------------
        # Save to CSV
        # -------------------------------------------------

        echo "$MATRIXSIZE,$TILESIZE,$INSTRUCTIONS,$L1D_MISSES,$SPEEDUP" >> "$CSV"

        echo ""
        echo "Captured:"
        echo "  Instructions : $INSTRUCTIONS"
        echo "  L1-D misses  : $L1D_MISSES"
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