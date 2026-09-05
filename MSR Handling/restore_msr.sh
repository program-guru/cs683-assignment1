#!/bin/bash
# restore_msr.sh — writes back the original backed-up values.
BACKUP="$HOME/msr_backup/msr_0x1a4_backup.txt"
if [ ! -f "$BACKUP" ]; then
  echo "ERROR: No backup found. Cannot restore."
  exit 1
fi

for i in 0 1 2 3 4 5 6 7; do
  orig=$(grep "^core${i}=" "$BACKUP" | cut -d= -f2)
  sudo wrmsr -p$i 0x1a4 "$orig"
  echo "core $i: restored to $orig"
done