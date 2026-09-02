#!/bin/bash
# Backs up MSR 0x1A4 (prefetch control) for all 8 cores into one file.
set -e

OUTFILE="$HOME/msr_backup/msr_0x1a4_backup.txt"
mkdir -p "$HOME/msr_backup"

echo "# MSR 0x1A4 backup - $(date)" > "$OUTFILE"
for i in 0 1 2 3 4 5 6 7; do
  val=$(sudo rdmsr -p$i 0x1a4)
  echo "core${i}=0x${val}" >> "$OUTFILE"
  echo "core $i original value: 0x$val"
done

echo "Backup saved to $OUTFILE"
cat "$OUTFILE"