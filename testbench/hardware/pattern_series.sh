#!/bin/bash
# Messreihe: erzeugt aus einem Testprogramm Varianten mit verschiedenen Werten an einem Byte (z.B. Bitmap-
# Füllmuster), nimmt jede Variante auf dem echten C64 auf und rendert sie mit Emu64.
#
#   ./pattern_series.sh -p <programm.prg> -o <offset> -v "00 40 80 c0" -n <name> -m <vicmodell 0|1>
#
#   -e <wert>     erwarteter alter Wert am Offset (Schutz gegen falschen Offset)
#   -l <name>     Name der Emu64-Variante (Standard: emu64). Mehrere Varianten (andere Binary per EMU64_BIN
#                 oder andere Umgebungsvariablen) mit verschiedenen Namen nacheinander rendern und vergleichen.
#   --no-hw       nur Emu64 rendern (Aufnahmen existieren schon)
#   --no-emu      nur auf der Hardware aufnehmen
#
# Ablage: $CAPTURE_DIR/<name>/<wert>/frame_NN.png (Hardware), .../<label>-m<modell>.png (Emu64),
#         .../test.prg (die Variante). Danach vergleichen, z.B.:
#   ./hwcompare.py compare --region E+B=188:195,104:124 $CAPTURE_DIR/<name>/80:alt=.../a-m0.png,neu=.../b-m0.png

set -u
source "$(dirname "${BASH_SOURCE[0]}")/hw_config.sh"

PRG=""; OFFSET=""; VALUES=""; NAME=""; MODEL=""; EXPECT=""; LABEL="emu64"; DO_HW=1; DO_EMU=1
while [ $# -gt 0 ]; do
    case "$1" in
        -p) PRG="$2"; shift 2 ;;
        -o) OFFSET="$2"; shift 2 ;;
        -v) VALUES="$2"; shift 2 ;;
        -n) NAME="$2"; shift 2 ;;
        -m) MODEL="$2"; shift 2 ;;
        -e) EXPECT="$2"; shift 2 ;;
        -l) LABEL="$2"; shift 2 ;;
        --no-hw) DO_HW=0; shift ;;
        --no-emu) DO_EMU=0; shift ;;
        -h|--help) sed -n '2,17p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "Unbekannter Parameter: $1" >&2; exit 2 ;;
    esac
done
[ -n "$PRG" ] && [ -n "$OFFSET" ] && [ -n "$VALUES" ] && [ -n "$NAME" ] && [ -n "$MODEL" ] || \
    { echo "Aufruf: $0 -p <prg> -o <offset> -v \"<werte>\" -n <name> -m <0|1> (siehe -h)" >&2; exit 2; }
[ "$DO_HW" -eq 1 ] && hw_require u2 video

for v in $VALUES; do
    dir="$CAPTURE_DIR/$NAME/$v"
    mkdir -p "$dir"
    python3 "$HW_DIR/patch_prg.py" "$PRG" "$dir/test.prg" "$OFFSET" "0x$v" ${EXPECT:+--expect "0x$EXPECT"} || exit 1
    if [ "$DO_HW" -eq 1 ]; then
        echo "== $v: Hardware"
        "$HW_DIR/hw_run.sh" "$dir/test.prg" "$NAME/$v" || exit 1
    fi
done

# Emu64-Läufe sind unabhängig voneinander und laufen parallel (jeder mit eigenem HOME)
if [ "$DO_EMU" -eq 1 ]; then
    echo "== Emu64 ($LABEL, VIC-Modell $MODEL)"
    for v in $VALUES; do
        dir="$CAPTURE_DIR/$NAME/$v"
        "$HW_DIR/emu_screenshot.sh" "$dir/test.prg" "$dir/$LABEL-m$MODEL.png" "$MODEL" &
    done
    wait
fi
echo "Fertig: $CAPTURE_DIR/$NAME"
