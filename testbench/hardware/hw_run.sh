#!/bin/bash
# Startet ein PRG auf dem echten C64 (über die 1541 Ultimate II+, REST runners:run_prg) und nimmt danach
# Bilder mit dem Video-Grabber auf. Die Bilder werden später von hwcompare.py gemittelt.
#
#   ./hw_run.sh <programm.prg> <name>       Aufnahmen nach $CAPTURE_DIR/<name>/frame_NN.png
#   ./hw_run.sh -n 10 -w 5 x.prg test       10 Bilder, 5 s warten
#   ./hw_run.sh --capture-only <name>       nur aufnehmen (Programm läuft schon oder wurde von Hand gestartet)
#
# Hinweise:
#  - run_prg lädt das Programm per DMA und startet es mit RUN. Ein eingestelltes Cartridge-Menü
#    (z.B. Retro Replay) wird dabei umgangen. An der Ultimate wird nichts umkonfiguriert.
#  - Der Kontrast des Grabbers wird für die Aufnahme auf $MEASURE_CONTRAST gesetzt und danach zurückgestellt.
#  - In <name>/meta.txt stehen Programm, Zeit, Gerät und Kontrast der Aufnahme.

set -u
source "$(dirname "${BASH_SOURCE[0]}")/hw_config.sh"

CAPTURE_ONLY=0
while [ $# -gt 0 ]; do
    case "$1" in
        -n) FRAMES="$2"; shift 2 ;;
        -w) SETTLE="$2"; shift 2 ;;
        --capture-only) CAPTURE_ONLY=1; shift ;;
        -h|--help) sed -n '2,15p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) break ;;
    esac
done

if [ "$CAPTURE_ONLY" -eq 1 ]; then
    [ $# -eq 1 ] || { echo "Aufruf: $0 --capture-only <name>" >&2; exit 2; }
    PRG=""; NAME="$1"
    hw_require video
else
    [ $# -eq 2 ] || { echo "Aufruf: $0 [-n bilder] [-w sekunden] <programm.prg> <name>" >&2; exit 2; }
    PRG="$1"; NAME="$2"
    [ -f "$PRG" ] || { echo "Fehler: $PRG nicht gefunden" >&2; exit 1; }
    hw_require u2 video
fi

OUT="$CAPTURE_DIR/$NAME"
mkdir -p "$OUT"
rm -f "$OUT"/frame_*.png

if [ -n "$PRG" ]; then
    reply=$(curl -s -m 30 -X POST --data-binary @"$PRG" -H "Content-Type: application/octet-stream" \
            "http://$U2_HOST/v1/runners:run_prg") || { echo "Fehler: Ultimate unter $U2_HOST nicht erreichbar" >&2; exit 1; }
    if ! echo "$reply" | tr -d ' \n' | grep -q '"errors":\[\]'; then
        echo "Fehler: run_prg meldet: $reply" >&2
        exit 1
    fi
    sleep "$SETTLE"
fi

python3 "$HW_DIR/grabber_ctrl.py" "$VIDEO_DEV" std "$VIDEO_STANDARD" || exit 1

OLD_CONTRAST=""
if [ -n "$MEASURE_CONTRAST" ]; then
    OLD_CONTRAST=$(python3 "$HW_DIR/grabber_ctrl.py" "$VIDEO_DEV" get contrast) || exit 1
    python3 "$HW_DIR/grabber_ctrl.py" "$VIDEO_DEV" set contrast "$MEASURE_CONTRAST" || exit 1
    trap 'python3 "$HW_DIR/grabber_ctrl.py" "$VIDEO_DEV" set contrast "$OLD_CONTRAST"' EXIT
    sleep 1
fi

timeout 60 ffmpeg -nostdin -hide_banner -loglevel error -f v4l2 -i "$VIDEO_DEV" -frames:v "$FRAMES" -y "$OUT/frame_%02d.png"
rc=$?

{
    echo "programm: ${PRG:-(von Hand gestartet)}"
    echo "zeit: $(date '+%Y-%m-%d %H:%M:%S')"
    echo "geraet: $VIDEO_DEV ($(cat /sys/class/video4linux/"$(basename "$VIDEO_DEV")"/name 2>/dev/null))"
    echo "norm: $VIDEO_STANDARD"
    echo "kontrast: ${MEASURE_CONTRAST:-unverändert}"
    echo "bilder: $FRAMES"
} > "$OUT/meta.txt"

if [ $rc -ne 0 ]; then
    echo "Fehler: Aufnahme mit ffmpeg fehlgeschlagen (Exitcode $rc)" >&2
    exit 1
fi
echo "$(ls "$OUT"/frame_*.png | wc -l) Bilder in $OUT"
