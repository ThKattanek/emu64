#!/bin/bash
# Gemeinsame Einstellungen der Hardware-Messskripte (echter C64 über 1541 Ultimate II+ und Video-Grabber).
# Eigene Werte NICHT hier ändern, sondern in testbench/config.local.sh (wird nicht eingecheckt)
# oder per Umgebungsvariable setzen, z.B.: U2_HOST=10.0.0.5 VIDEO_DEV=/dev/video4 ./hw_run.sh ...

HW_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$HW_DIR/../config.sh"

# Adresse der Ultimate (REST-API unter http://<U2_HOST>/v1). Pflicht, kein Standardwert.
U2_HOST="${U2_HOST:-}"

# Video4Linux-Gerät des Grabbers (z.B. /dev/video4). Leer lassen: Dann wird das Gerät mit dem Namen VIDEO_NAME
# gesucht (Standard stk1160, EasyCAP). Die Nummer ändert sich beim Umstecken, der Name nicht.
VIDEO_DEV="${VIDEO_DEV:-}"
VIDEO_NAME="${VIDEO_NAME:-stk1160}"

# Ablage der Aufnahmen (unter build/ und damit git-ignoriert)
CAPTURE_DIR="${CAPTURE_DIR:-$REPO_DIR/build/hardware_captures}"

# Anzahl gemittelter Bilder pro Aufnahme und Wartezeit nach dem Programmstart in Sekunden
FRAMES="${FRAMES:-20}"
SETTLE="${SETTLE:-7}"

# Kontrast des Grabbers während der Aufnahme. Beim stk1160 übersteuert Weiß mit dem Standardwert 64,
# mit 48 bleibt der volle Helligkeitsbereich erhalten. Nach der Aufnahme wird der alte Wert wiederhergestellt.
# Leer lassen, um den Kontrast nicht anzufassen.
MEASURE_CONTRAST="${MEASURE_CONTRAST-48}"

# config.sh hat config.local.sh schon eingelesen. Erneut einlesen, damit dort gesetzte Hardware-Werte
# Vorrang vor den Standardwerten oben haben.
[ -f "$TESTBENCH_DIR/config.local.sh" ] && source "$TESTBENCH_DIR/config.local.sh"

# Grabber über den Namen finden, wenn kein Gerät fest eingetragen ist (erstes passendes /dev/videoN)
if [ -z "$VIDEO_DEV" ] && [ -n "$VIDEO_NAME" ]; then
    for d in $(ls -d /sys/class/video4linux/video* 2>/dev/null | sort -V); do
        if [ "$(cat "$d/name" 2>/dev/null)" = "$VIDEO_NAME" ]; then
            VIDEO_DEV="/dev/$(basename "$d")"
            break
        fi
    done
fi

# Prüft die benötigten Einstellungen. Aufruf: hw_require u2 video  (nur die genannten werden geprüft)
hw_require()
{
    local missing=0
    if [[ " $* " == *" u2 "* ]] && [ -z "$U2_HOST" ]; then
        echo "Fehler: U2_HOST ist nicht gesetzt (Adresse der Ultimate, in testbench/config.local.sh eintragen)." >&2
        missing=1
    fi
    if [[ " $* " == *" video "* ]] && [ -z "$VIDEO_DEV" ]; then
        echo "Fehler: Kein Grabber \"$VIDEO_NAME\" gefunden (angeschlossen?). VIDEO_DEV oder VIDEO_NAME in testbench/config.local.sh eintragen. Vorhandene Geräte:" >&2
        for d in /sys/class/video4linux/video*; do
            [ -e "$d/name" ] && echo "  /dev/$(basename "$d"): $(cat "$d/name")" >&2
        done
        missing=1
    fi
    [ "$missing" -eq 0 ] || exit 1
}
