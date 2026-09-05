#!/bin/bash
# Disables all 4 documented prefetcher bits on all 8 cores.
# Run backup_msr.sh BEFORE this, so you have a restore point.
set -e

BACKUP="$HOME/msr_backup/msr_0x1a4_backup.txt"
if [ ! -f "$BACKUP" ]; then
  echo "ERROR: No backup found at $BACKUP. Run backup_msr.sh first. Aborting."
  exit 1
fi

for i in 0 1 2 3 4 5 6 7; do
  sudo wrmsr -p$i 0x1a4 0xf
  echo "core $i: wrote 0xf"
done

echo "Done. Run read_msr.sh to verify."