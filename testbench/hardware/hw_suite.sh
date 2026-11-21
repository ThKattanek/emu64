#!/bin/bash
# Nimmt den festen Messsatz (suite_tests.txt) auf dem gerade angeschlossenen C64 auf und erkennt den VIC-Typ.
# Dauer etwa 3 Minuten. Danach mit hw_suite_eval.py auswerten.
#
#   ./hw_suite.sh <gerät>          z.B. ./hw_suite.sh 8565R2_2088
#
# Ablage: $CAPTURE_DIR/suite/<gerät>/<test>/frame_NN.png, erkannter VIC-Typ in .../<gerät>/model.txt.
# Den Gerätenamen am besten aus Revision und Datumscode vom Chip-Aufdruck bilden. Ein vorhandener Satz
# mit gleichem Namen wird überschrieben (z.B. für eine zweite Messung nach dem Warmlaufen einen neuen Namen
# wie 8565R2_2088_warm nehmen).

set -u
source "$(dirname "${BASH_SOURCE[0]}")/hw_config.sh"

[ $# -eq 1 ] || { echo "Aufruf: $0 <gerät>" >&2; exit 2; }
DEVICE="$1"
hw_require u2 video

TESTPROGS="$(cd "$VICE_TESTBENCH/.." && pwd)"
OUT="$CAPTURE_DIR/suite/$DEVICE"
mkdir -p "$OUT"
echo "Gerät: $DEVICE   Start: $(date '+%Y-%m-%d %H:%M:%S')" > "$OUT/info.txt"

while read -r path cycles; do
    case "$path" in ''|'#'*) continue ;; esac
    name="$(basename "$path" .prg)"
    echo -n "$name: "
    "$HW_DIR/hw_run.sh" "$TESTPROGS/$path" "suite/$DEVICE/$name" < /dev/null || exit 1
done < "$HW_DIR/suite_tests.txt"

echo "Ende: $(date '+%Y-%m-%d %H:%M:%S')" >> "$OUT/info.txt"
python3 "$HW_DIR/hw_suite_eval.py" --identify "$DEVICE"
