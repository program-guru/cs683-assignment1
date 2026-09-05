#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

bash "$SCRIPT_DIR/GeneralRun.sh" naive
bash "$SCRIPT_DIR/GeneralRun.sh" reorder
bash "$SCRIPT_DIR/GeneralRun.sh" unroll
bash "$SCRIPT_DIR/SIMDRun.sh"
bash "$SCRIPT_DIR/TileRun.sh"
python3 "$SCRIPT_DIR/analyze.py"
