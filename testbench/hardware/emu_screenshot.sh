#!/bin/bash
# Erzeugt mit Emu64 einen Screenshot eines Programms, mit denselben Einstellungen wie die VICE-Testbench
# (Palette 7, kein Filter). Jeder Aufruf bekommt ein eigenes HOME, damit parallele Aufrufe sich nicht über
# die gemeinsame emu64.ini gegenseitig das VIC-Modell verstellen.
#
#   ./emu_screenshot.sh <programm.prg> <ausgabe.png> <vicmodell 0|1> [zyklen]
#
#   vicmodell: 0 = MOS 6569, 1 = MOS 8565. Zyklen Standard: 35000000.
#   Andere Binary: EMU64_BIN=/pfad/emu64 ./emu_screenshot.sh ...

set -u
source "$(dirname "${BASH_SOURCE[0]}")/hw_config.sh"

[ $# -ge 3 ] || { echo "Aufruf: $0 <programm.prg> <ausgabe.png> <vicmodell 0|1> [zyklen]" >&2; exit 2; }
PRG="$1"; OUT="$2"; MODEL="$3"; CYCLES="${4:-35000000}"
[ -f "$PRG" ] || { echo "Fehler: $PRG nicht gefunden" >&2; exit 1; }
[ -x "$EMU64_BIN" ] || { echo "Fehler: Emu64 Binary $EMU64_BIN nicht gefunden" >&2; exit 1; }

H=$(mktemp -d)
trap 'rm -rf "$H"' EXIT
rm -f "$OUT"
HOME="$H" "$EMU64_BIN" --multiple-instance --reset-ini --set-vicmodel "$MODEL" --video-filter-off \
    --double-texture-off --set-palette 7 --warp --debugcart --nogui --limitcycles "$CYCLES" \
    --exitscreenshot "$OUT" --autostart "$PRG" > /dev/null 2>&1
[ -f "$OUT" ] || { echo "Fehler: kein Screenshot erzeugt ($PRG)" >&2; exit 1; }
