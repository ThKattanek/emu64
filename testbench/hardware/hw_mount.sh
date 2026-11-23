#!/bin/bash
# Legt ein Disk-Image vom PC in ein Laufwerk der 1541 Ultimate II+ ein (REST drives:mount) oder wirft es aus.
#
#   ./hw_mount.sh <image.d64>               in Laufwerk A einlegen (schreibgeschützt)
#   ./hw_mount.sh -a <image.d64>            einlegen und erstes Programm starten (LOAD"*",8,1 und RUN)
#   ./hw_mount.sh -a -f "INTRO" x.d64       einlegen und Programm INTRO starten
#   ./hw_mount.sh -d b -m rw <image.d64>    in Laufwerk B, Schreibzugriffe landen im Image auf der Ultimate
#   ./hw_mount.sh --eject [-d a]            Image auswerfen
#   ./hw_mount.sh --status                  Zustand der Laufwerke anzeigen
#
# Optionen:
#   -d a|b        Laufwerk der Ultimate (Standard a)
#   -m ro|rw|ul   ro = readonly (Standard), rw = readwrite, ul = unlinked (Änderungen nur im Speicher der Ultimate)
#   -t typ        Image-Typ (d64, g64, d71, g71, d81). Standard: aus der Dateiendung
#   -r            nach dem Einlegen das Laufwerk zurücksetzen (wie Floppy aus/ein)
#   -a            Autostart: C64 zurücksetzen, dann LOAD"<name>",<gerät>,1 und RUN
#   -f name       Dateiname für den Autostart (Standard *), Kleinbuchstaben werden zu Großbuchstaben
#
# Hinweise:
#  - Das Image wird übertragen, die Datei auf dem PC bleibt unverändert (auch bei rw).
#  - Ohne -a wird der C64 nicht zurückgesetzt. Danach z.B. LOAD"*",8,1 von Hand eingeben.
#  - Autostart: Ein kleines BASIC-Programm wird per runners:run_prg gestartet (umgeht ein Cartridge-Menü wie
#    Retro Replay). Es schreibt LOAD und RUN auf den Bildschirm und legt HOME, RETURN, RETURN in den
#    Tastaturpuffer. Programme, die nicht mit RUN starten (z.B. SYS 49152), damit nicht automatisch.
#  - Die Ultimate-1541 ist nicht zyklusgenau, für Floppy-Timing ist eine echte 1541 die Referenz.

set -u
source "$(dirname "${BASH_SOURCE[0]}")/hw_config.sh"

DRIVE="a"
MODE="readonly"
TYPE=""
ACTION="mount"
RESET_DRIVE=0
AUTOSTART=0
FILENAME="*"

usage() { sed -n '2,25p' "$0" | sed 's/^# \{0,1\}//'; }

while [ $# -gt 0 ]; do
    case "$1" in
        -d) DRIVE="${2,,}"; shift 2 ;;
        -m) case "$2" in
                ro|readonly) MODE="readonly" ;;
                rw|readwrite) MODE="readwrite" ;;
                ul|unlinked) MODE="unlinked" ;;
                *) echo "Fehler: unbekannter Modus \"$2\" (ro, rw, ul)" >&2; exit 2 ;;
            esac
            shift 2 ;;
        -t) TYPE="${2,,}"; shift 2 ;;
        -r) RESET_DRIVE=1; shift ;;
        -a) AUTOSTART=1; shift ;;
        -f) FILENAME="$2"; shift 2 ;;
        --eject) ACTION="eject"; shift ;;
        --status) ACTION="status"; shift ;;
        -h|--help) usage; exit 0 ;;
        -*) echo "Fehler: unbekannte Option $1" >&2; usage >&2; exit 2 ;;
        *) break ;;
    esac
done

case "$DRIVE" in
    a|b) ;;
    *) echo "Fehler: Laufwerk muss a oder b sein" >&2; exit 2 ;;
esac

hw_require u2
API="http://$U2_HOST/v1"

# Prüft die JSON-Antwort der Ultimate auf eine leere Fehlerliste
check_reply()
{
    local what="$1" reply="$2"
    if ! echo "$reply" | tr -d ' \n' | grep -q '"errors":\[\]'; then
        echo "Fehler: $what meldet: $reply" >&2
        exit 1
    fi
}

# Holt einen Wert des Laufwerks aus GET /v1/drives. Aufruf: drive_info <a|b> <schlüssel>
drive_info()
{
    curl -s -m 10 "$API/drives" | python3 -c '
import json, sys
drive, key = sys.argv[1], sys.argv[2]
for entry in json.load(sys.stdin).get("drives", []):
    if drive in entry:
        print(entry[drive].get(key, ""))
        break
' "$1" "$2"
}

# Erzeugt das Autostart-Programm (tokenisiertes BASIC, ab $0801):
#   10 PRINT"{CLR}LOAD"CHR$(34)"<name>"CHR$(34)",<gerät>,1":PRINT:PRINT:PRINT:PRINT:PRINT"RUN"
#      :POKE631,19:POKE632,13:POKE633,13:POKE198,3
# LOAD steht in Zeile 0, RUN in Zeile 5. Die Kernal-Meldungen beginnen mit RETURN: Zeile 1 bleibt leer, dann
# SEARCHING, LOADING und READY. (Zeilen 2-4). Danach steht der Cursor auf Zeile 5, das zweite RETURN aus dem
# Tastaturpuffer führt dort RUN aus.
make_autostart_prg()
{
    python3 - "$1" "$2" "$3" <<'EOF'
import sys
name, device, out = sys.argv[1].upper(), int(sys.argv[2]), sys.argv[3]
if not name or len(name) > 16 or any(not (0x20 <= ord(c) <= 0x5f) or c == '"' for c in name):
    sys.exit('Fehler: Dateiname muss 1-16 Zeichen lang sein (Buchstaben, Ziffern, Satzzeichen, kein ")')
PRINT, CHR, POKE = b"\x99", b"\xc7", b"\x97"
q = CHR + b"(34)"
body = (PRINT + b'"\x93LOAD"' + q + b'"' + name.encode("ascii") + b'"' + q + b'",' + str(device).encode() + b',1"'
        + (b":" + PRINT) * 5 + b'"RUN"'
        + b":" + POKE + b"631,19:" + POKE + b"632,13:" + POKE + b"633,13:" + POKE + b"198,3")
line = (10).to_bytes(2, "little") + body + b"\x00"
next_addr = 0x0801 + 2 + len(line)
prg = (0x0801).to_bytes(2, "little") + next_addr.to_bytes(2, "little") + line + b"\x00\x00"
open(out, "wb").write(prg)
EOF
}

case "$ACTION" in
    status)
        reply=$(curl -s -m 10 "$API/drives") || { echo "Fehler: Ultimate unter $U2_HOST nicht erreichbar" >&2; exit 1; }
        if command -v python3 >/dev/null; then
            echo "$reply" | python3 -m json.tool
        else
            echo "$reply"
        fi
        ;;

    eject)
        [ $# -eq 0 ] || { echo "Aufruf: $0 --eject [-d a|b]" >&2; exit 2; }
        reply=$(curl -s -m 10 -X PUT "$API/drives/$DRIVE:remove") || { echo "Fehler: Ultimate unter $U2_HOST nicht erreichbar" >&2; exit 1; }
        check_reply "drives:remove" "$reply"
        echo "Laufwerk ${DRIVE^^}: Image ausgeworfen"
        ;;

    mount)
        [ $# -eq 1 ] || { echo "Aufruf: $0 [-d a|b] [-m ro|rw|ul] [-t typ] [-r] [-a [-f name]] <image>" >&2; exit 2; }
        IMAGE="$1"
        [ -f "$IMAGE" ] || { echo "Fehler: $IMAGE nicht gefunden" >&2; exit 1; }
        if [ -z "$TYPE" ]; then
            TYPE="${IMAGE##*.}"
            TYPE="${TYPE,,}"
        fi
        case "$TYPE" in
            d64|g64|d71|g71|d81) ;;
            *) echo "Fehler: Image-Typ \"$TYPE\" wird nicht unterstützt (d64, g64, d71, g71, d81; sonst -t angeben)" >&2; exit 1 ;;
        esac

        # Autostart-Programm vorab erzeugen, damit ein falscher Dateiname vor dem Einlegen auffällt
        if [ "$AUTOSTART" -eq 1 ]; then
            TMP_DIR=$(mktemp -d)
            trap 'rm -rf "$TMP_DIR"' EXIT
            make_autostart_prg "$FILENAME" 8 "$TMP_DIR/check.prg" || exit 1
        fi

        reply=$(curl -s -m 30 -X POST --data-binary @"$IMAGE" -H "Content-Type: application/octet-stream" \
                "$API/drives/$DRIVE:mount?type=$TYPE&mode=$MODE") || { echo "Fehler: Ultimate unter $U2_HOST nicht erreichbar" >&2; exit 1; }
        check_reply "drives:mount" "$reply"

        if [ "$RESET_DRIVE" -eq 1 ]; then
            reply=$(curl -s -m 10 -X PUT "$API/drives/$DRIVE:reset") || { echo "Fehler: Ultimate unter $U2_HOST nicht erreichbar" >&2; exit 1; }
            check_reply "drives:reset" "$reply"
        fi
        echo "Laufwerk ${DRIVE^^}: $(basename "$IMAGE") eingelegt ($TYPE, $MODE)"

        if [ "$AUTOSTART" -eq 1 ]; then
            if [ "$(drive_info "$DRIVE" enabled)" != "True" ]; then
                echo "Fehler: Laufwerk ${DRIVE^^} ist an der Ultimate ausgeschaltet, Autostart nicht möglich" >&2
                exit 1
            fi
            DEVICE=$(drive_info "$DRIVE" bus_id)
            [[ "$DEVICE" =~ ^[0-9]+$ ]] || { echo "Fehler: Gerätenummer von Laufwerk ${DRIVE^^} nicht lesbar" >&2; exit 1; }
            make_autostart_prg "$FILENAME" "$DEVICE" "$TMP_DIR/autostart.prg" || exit 1
            reply=$(curl -s -m 30 -X POST --data-binary @"$TMP_DIR/autostart.prg" -H "Content-Type: application/octet-stream" \
                    "$API/runners:run_prg") || { echo "Fehler: Ultimate unter $U2_HOST nicht erreichbar" >&2; exit 1; }
            check_reply "runners:run_prg" "$reply"
            echo "Autostart: LOAD\"${FILENAME^^}\",$DEVICE,1 und RUN"
        fi
        ;;
esac
