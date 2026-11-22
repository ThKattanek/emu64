#!/bin/bash
# Zeigt das Bild des Video-Grabbers (echter C64) live in einem Fenster auf dem Desktop, mit ffplay aus ffmpeg.
#
#   ./hw_view.sh                sichtbares C64-Bild (384x272 mit Rahmen), Fenster 702x544
#   ./hw_view.sh -z 2           doppelte Größe
#   ./hw_view.sh -f             Vollbild
#   ./hw_view.sh --full         ganzes Grabberbild statt nur des C64-Bildes (768x576, 4:3)
#   ./hw_view.sh --pixel        beim Vergrößern Pixel nicht glätten (harte Kanten)
#   ./hw_view.sh -d yadif       anderen Deinterlacer wählen (bwdif Standard, yadif, field, none)
#   ./hw_view.sh --hue -10      Farbton in Grad drehen (gegen Farbstich durch die PAL-Farbphase des C64)
#   ./hw_view.sh --sat 1.2      Farbsättigung (1 = unverändert)
#
# Eigene Standardwerte (z.B. Farbton deines C64) in testbench/config.local.sh eintragen:
#   HW_VIEW_HUE=-10
#   HW_VIEW_SAT=0.9
#   HW_VIEW_CROP="658:544:31:4"
#
# Ausschnitt: Der Bereich der Testbench-Screenshots (384x272, wie in Emu64 ohne Gesamtbild) liegt beim stk1160
# bei x = 1,714 * x_c64 + 31 und y = 2 * y_c64 + 4 (siehe MESSUNGEN.md). Bei einem anderen Grabber oder
# verschobenem Bild per Umgebungsvariable anpassen: HW_VIEW_CROP="breite:höhe:x:y" (in Grabber-Pixeln,
# y und höhe gerade, sonst vertauschen sich die Halbbilder).
#
# Deinterlacer: Der C64 liefert ein nicht verschachteltes Bild mit 288 Zeilen, der Grabber webt aber je zwei
# Halbbilder zu einem Vollbild. Bei Bewegung gibt das Kammartefakte. bwdif/yadif zeigen jedes Halbbild einzeln
# (50 Bilder/s). "field" verdoppelt einfach die Zeilen eines Halbbilds (am schnellsten, 25 Bilder/s), "none" aus.
#
# Farben: Das YUV-Bild des Grabbers wird hier mit BT.601 (PAL/SD) nach RGB umgerechnet. Ohne diese Umrechnung wählt
# SDL in ffplay die Farbmatrix selbst, das gibt leicht einen Rot-/Orangestich.
#
# Tasten im Fenster: q / Esc beenden, f Vollbild an/aus.
#
# Hinweis: Solange das Fenster offen ist, ist der Grabber belegt. hw_run.sh, hw_suite.sh und
# pattern_series.sh können dann nicht aufnehmen, vorher das Fenster schließen.

set -u
source "$(dirname "${BASH_SOURCE[0]}")/hw_config.sh"

ZOOM=1
FULLSCREEN=0
FULL=0
SCALE_FLAGS="bicubic"
DEINT="bwdif"
HUE="${HW_VIEW_HUE:-0}"     # Standardwerte aus testbench/config.local.sh (HW_VIEW_HUE, HW_VIEW_SAT)
SAT="${HW_VIEW_SAT:-1}"
CROP="${HW_VIEW_CROP:-658:544:31:4}"
while [ $# -gt 0 ]; do
    case "$1" in
        -z) ZOOM="$2"; shift 2 ;;
        -f) FULLSCREEN=1; shift ;;
        -d) DEINT="$2"; shift 2 ;;
        --hue) HUE="$2"; shift 2 ;;
        --sat) SAT="$2"; shift 2 ;;
        --full) FULL=1; shift ;;
        --pixel) SCALE_FLAGS="neighbor"; shift ;;
        -h|--help) sed -n '2,/^$/p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "Unbekannter Parameter: $1 (siehe -h)" >&2; exit 2 ;;
    esac
done

hw_require video
command -v ffplay > /dev/null || { echo "Fehler: ffplay nicht gefunden (Paket ffmpeg)." >&2; exit 1; }
python3 "$HW_DIR/grabber_ctrl.py" "$VIDEO_DEV" std "$VIDEO_STANDARD" || exit 1

case "$DEINT" in
    bwdif) DEINT_FILTER="bwdif=mode=send_field:parity=auto," ;;
    yadif) DEINT_FILTER="yadif=mode=send_field:parity=auto," ;;
    field) DEINT_FILTER="field=top," ;;
    none)  DEINT_FILTER="" ;;
    *) echo "Unbekannter Deinterlacer: $DEINT (bwdif, yadif, field, none)" >&2; exit 2 ;;
esac

# Die Grabber-Pixel sind nicht quadratisch: 720 Abtastwerte entsprechen 768 quadratischen Pixeln (4:3 bei 576 Zeilen)
if [ "$FULL" -eq 1 ]; then
    CROP_FILTER=""
    SRC_W=720; SRC_H=576
else
    IFS=: read -r SRC_W SRC_H CROP_X CROP_Y <<< "$CROP"
    if [ $((SRC_H % 2)) -ne 0 ] || [ $((CROP_Y % 2)) -ne 0 ]; then
        echo "Fehler: HW_VIEW_CROP: höhe und y müssen gerade sein (sonst vertauschen sich die Halbbilder)." >&2
        exit 2
    fi
    CROP_FILTER="crop=${SRC_W}:${SRC_H}:${CROP_X}:${CROP_Y},"
fi
W=$(awk -v w="$SRC_W" -v z="$ZOOM" 'BEGIN { printf "%d", int(w * 768 / 720 * z / 2 + 0.5) * 2 }')
H=$(awk -v h="$SRC_H" -v z="$ZOOM" 'BEGIN { printf "%d", int(h * z / 2 + 0.5) * 2 }')

# Farbton/Sättigung nur, wenn verändert
COLOR_FILTER=""
if [ "$HUE" != "0" ] || [ "$SAT" != "1" ]; then COLOR_FILTER="hue=h=${HUE}:s=${SAT},"; fi

# Erst zuschneiden (Halbbilder bleiben erhalten), dann deinterlacen, Farbton (im YUV-Farbraum), dann skalieren und
# dabei mit BT.601 nach RGB umrechnen (format=bgra lässt scale die Umrechnung selbst machen)
ARGS=(-hide_banner -loglevel error -f v4l2 -i "$VIDEO_DEV" -an
      -fflags nobuffer -flags low_delay -framedrop
      -vf "${CROP_FILTER}${DEINT_FILTER}${COLOR_FILTER}scale=${W}:${H}:flags=${SCALE_FLAGS}:in_color_matrix=bt601:in_range=tv:out_range=full,format=bgra"
      -window_title "C64 ($VIDEO_DEV)")
[ "$FULLSCREEN" -eq 1 ] && ARGS+=(-fs)

exec ffplay "${ARGS[@]}"
