#!/bin/bash
# Zeigt das Bild des Video-Grabbers (echter C64) live in einem Fenster auf dem Desktop, mit ffplay aus ffmpeg.
#
#   ./hw_view.sh                Fenster 768x576 (4:3, PAL)
#   ./hw_view.sh -z 2           doppelte Größe (1536x1152)
#   ./hw_view.sh -f             Vollbild
#   ./hw_view.sh --pixel        beim Vergrößern Pixel nicht glätten (harte Kanten)
#
# Tasten im Fenster: q / Esc beenden, f Vollbild an/aus.
#
# Hinweis: Solange das Fenster offen ist, ist der Grabber belegt. hw_run.sh, hw_suite.sh und
# pattern_series.sh können dann nicht aufnehmen, vorher das Fenster schließen.

set -u
source "$(dirname "${BASH_SOURCE[0]}")/hw_config.sh"

ZOOM=1
FULLSCREEN=0
SCALE_FLAGS="bicubic"
while [ $# -gt 0 ]; do
    case "$1" in
        -z) ZOOM="$2"; shift 2 ;;
        -f) FULLSCREEN=1; shift ;;
        --pixel) SCALE_FLAGS="neighbor"; shift ;;
        -h|--help) sed -n '2,13p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "Unbekannter Parameter: $1 (siehe -h)" >&2; exit 2 ;;
    esac
done

hw_require video
command -v ffplay > /dev/null || { echo "Fehler: ffplay nicht gefunden (Paket ffmpeg)." >&2; exit 1; }

# PAL-Bild des Grabbers (720x576, nicht quadratische Pixel) auf 4:3 bringen
W=$(awk -v z="$ZOOM" 'BEGIN { printf "%d", 768 * z }')
H=$(awk -v z="$ZOOM" 'BEGIN { printf "%d", 576 * z }')

ARGS=(-hide_banner -loglevel error -f v4l2 -i "$VIDEO_DEV" -an
      -fflags nobuffer -flags low_delay -framedrop
      -vf "scale=${W}:${H}:flags=${SCALE_FLAGS}"
      -window_title "C64 ($VIDEO_DEV)")
[ "$FULLSCREEN" -eq 1 ] && ARGS+=(-fs)

exec ffplay "${ARGS[@]}"
