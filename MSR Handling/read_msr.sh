#!/bin/bash
# Reads current MSR 0x1A4 values on all cores and diffs against backup.
set -e

BACKUP="$HOME/msr_backup/msr_0x1a4_backup.txt"

echo "Current MSR 0x1A4 values:"
for i in 0 1 2 3 4 5 6 7; do
  val=$(sudo rdmsr -p$i 0x1a4)
  echo "core $i: 0x$val"
done

if [ -f "$BACKUP" ]; then
  echo ""
  echo "--- Comparing against backup ($BACKUP) ---"
  for i in 0 1 2 3 4 5 6 7; do
    orig=$(grep "^core${i}=" "$BACKUP" | cut -d= -f2)
    curr="0x$(sudo rdmsr -p$i 0x1a4)"
    if [ "$orig" == "$curr" ]; then
      echo "core $i: UNCHANGED ($curr)"
    else
      echo "core $i: CHANGED  original=$orig  current=$curr"
    fi
  done
else
  echo "No backup file found at $BACKUP to compare against."
fi