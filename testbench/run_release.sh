#!/bin/bash
# Führt die VICE Testbench für Emu64 aus (parallel) und legt das Ergebnis unter results/ ab.
#
#   ./run_release.sh                      kompletter Lauf, Version aus "git describe"
#   ./run_release.sh -v 5.2.0             Version vorgeben
#   ./run_release.sh -j 8                 8 parallele Worker
#   ./run_release.sh -g drive/scanner     nur Tests, deren Pfad "drive/scanner" enthält (Teillauf)
#   ./run_release.sh -e /pfad/emu64       andere Binary testen
#
# Ein kompletter Lauf wird als results/emu64-<Version>-<Datum>-result.txt gespeichert und danach wird
# results/index.html neu erzeugt. Teilläufe (-g) landen in results/partial/ und nicht in der Tabelle.
#
# Wichtig zum Aufbau:
#  - Die VICE testprogs werden nach $WORK_DIR gespiegelt. Die Tests schreiben auf Disk Images und legen
#    Screenshots ab, das VICE Verzeichnis bleibt damit unverändert und jeder Lauf startet mit sauberen Images.
#  - Jeder Worker bekommt ein eigenes testbench Verzeichnis (eigene Testliste, eigene Ergebnisdatei) und
#    ein eigenes HOME (die emu64.ini ist sonst für alle Prozesse dieselbe Datei).
#  - Emu64 wird immer mit --multiple-instance gestartet. Ohne den Parameter reicht Emu64 die Argumente an
#    eine bereits laufende Instanz weiter und endet sofort mit Exitcode 0, die Testbench wertet das als "ok".

set -u

source "$(dirname "${BASH_SOURCE[0]}")/config.sh"

VERSION=""
GROUPS_FILTER=()
FORCE=0
NO_SYNC=0

usage()
{
    sed -n '2,17p' "$0" | sed 's/^# \{0,1\}//'
    echo "  -f, --force      vorhandenes Ergebnis gleichen Namens überschreiben"
    echo "      --no-sync    die Kopie der VICE testprogs nicht aktualisieren"
    exit "${1:-0}"
}

while [ $# -gt 0 ]; do
    case "$1" in
        -v|--version) VERSION="$2"; shift 2 ;;
        -j|--jobs)    JOBS="$2"; shift 2 ;;
        -g|--groups)  IFS=',' read -r -a GROUPS_FILTER <<< "$2"; shift 2 ;;
        -e|--emu)     EMU64_BIN="$2"; shift 2 ;;
        -f|--force)   FORCE=1; shift ;;
        --no-sync)    NO_SYNC=1; shift ;;
        -h|--help)    usage 0 ;;
        *) echo "Unbekannter Parameter: $1" >&2; usage 1 ;;
    esac
done

die() { echo "Fehler: $*" >&2; exit 1; }

# --- Voraussetzungen ---------------------------------------------------------------------------------------

[ -x "$EMU64_BIN" ] || die "Emu64 Binary nicht gefunden: $EMU64_BIN (siehe config.sh / -e)"
[ -f "$VICE_TESTBENCH/testbench.sh" ] || die "VICE Testbench nicht gefunden: $VICE_TESTBENCH (siehe config.sh)"
for f in emu64-testlist.txt emu64-hooks.sh Makefile.config cmpscreens; do
    [ -e "$VICE_TESTBENCH/$f" ] || die "$f fehlt in $VICE_TESTBENCH. Dort einmal 'make prereq' ausführen."
done
for t in petcat c1541 acme micro64disktool python3 rsync timeout; do
    command -v "$t" > /dev/null || die "Programm '$t' wird gebraucht, ist aber nicht installiert."
done
[ "$JOBS" -ge 1 ] 2> /dev/null || die "Ungültige Worker Anzahl: $JOBS"

# --- Name des Laufs ----------------------------------------------------------------------------------------

if [ -z "$VERSION" ]; then
    VERSION="$(git -C "$REPO_DIR" describe --tags --always --dirty 2> /dev/null || echo unbekannt)"
fi
VERSION="$(echo "$VERSION" | tr -c 'A-Za-z0-9._+\n-' '_')"
DATE="$(date +%Y%m%d)"
RUN_NAME="emu64-${VERSION}-${DATE}"

if [ ${#GROUPS_FILTER[@]} -gt 0 ]; then
    PARTIAL=1
    OUT_DIR="$RESULTS_DIR/partial"
else
    PARTIAL=0
    OUT_DIR="$RESULTS_DIR"
fi
OUT_FILE="$OUT_DIR/${RUN_NAME}-result.txt"
OUT_META="$OUT_DIR/${RUN_NAME}.meta"

if [ -e "$OUT_FILE" ] && [ "$FORCE" = 0 ]; then
    die "$OUT_FILE existiert schon. Mit --force überschreiben oder mit -v eine andere Version angeben."
fi

RUN_DIR="$WORK_DIR/run-$RUN_NAME"
MIRROR="$WORK_DIR/vice-testprogs"
mkdir -p "$WORK_DIR" "$OUT_DIR"
rm -rf "$RUN_DIR"
mkdir -p "$RUN_DIR"

echo "Emu64:    $EMU64_BIN"
echo "Version:  $VERSION ($DATE)"
echo "Worker:   $JOBS"
[ "$PARTIAL" = 1 ] && echo "Teillauf: ${GROUPS_FILTER[*]}"

# --- Kopie der VICE testprogs ------------------------------------------------------------------------------

VICE_TESTPROGS="$(dirname "$VICE_TESTBENCH")"
if [ "$NO_SYNC" = 0 ] || [ ! -d "$MIRROR" ]; then
    echo "Aktualisiere Kopie der VICE testprogs in $MIRROR ..."
    mkdir -p "$MIRROR"
    rsync -a --delete --exclude='/testbench/results/' "$VICE_TESTPROGS/" "$MIRROR/" || die "rsync fehlgeschlagen"
fi

# --- Testliste aufteilen -----------------------------------------------------------------------------------

echo "Teile die Testliste auf ..."
"$TESTBENCH_DIR/split_testlist.py" "$MIRROR/testbench/emu64-testlist.txt" "$JOBS" "$RUN_DIR/chunks" \
    "${GROUPS_FILTER[@]}" > "$RUN_DIR/chunks.txt" || die "Aufteilen der Testliste fehlgeschlagen"
WORKERS=$(wc -l < "$RUN_DIR/chunks.txt")
TOTAL_TESTS=$(awk '{s += $2} END {print s}' "$RUN_DIR/chunks.txt")
echo "$TOTAL_TESTS Tests auf $WORKERS Worker verteilt."

# --- Worker vorbereiten und starten ------------------------------------------------------------------------

for i in $(seq 1 "$WORKERS"); do
    W="$RUN_DIR/w$i"
    mkdir -p "$W/testbench" "$W/home" "$W/bin"

    # Die Tests greifen mit ../<verzeichnis> auf ihre Programme zu, deshalb alle Nachbarverzeichnisse verlinken
    for d in "$MIRROR"/*; do
        b="$(basename "$d")"
        [ "$b" = "testbench" ] && continue
        ln -s "$d" "$W/$b"
    done

    # testbench Verzeichnis: alles verlinken, außer den Dateien, die pro Worker eigen sind
    for f in "$MIRROR/testbench"/*; do
        b="$(basename "$f")"
        case "$b" in
            Makefile|emu64-testlist.txt|emu64-result.txt|results) continue ;;
        esac
        ln -s "$f" "$W/testbench/$b"
    done
    printf 'prereq:\n\t@true\n' > "$W/testbench/Makefile"     # testbench.sh ruft "make prereq" auf, das ist bereits erledigt
    cp "$RUN_DIR/chunks/chunk-$i.txt" "$W/testbench/emu64-testlist.txt"

    cat > "$W/bin/emu64" <<EOF
#!/bin/bash
export HOME="$W/home"
exec timeout $EMU64_TIMEOUT "$EMU64_BIN" --multiple-instance "\$@"
EOF
    chmod +x "$W/bin/emu64"
done

START=$SECONDS
for i in $(seq 1 "$WORKERS"); do
    W="$RUN_DIR/w$i"
    (
        cd "$W/testbench" || exit 1
        EMUDIR="$W/bin/" ./testbench.sh emu64 $TESTBENCH_FLAGS > "$W/log.txt" 2>&1
        echo $? > "$W/exitcode"
    ) &
done

# Fortschritt anzeigen, bis alle Worker fertig sind
while [ "$(jobs -rp | wc -l)" -gt 0 ]; do
    sleep 20
    DONE=0
    for i in $(seq 1 "$WORKERS"); do
        R="$RUN_DIR/w$i/testbench/emu64-result.txt"
        [ -f "$R" ] && DONE=$(( DONE + $(grep -vc '^#' "$R") ))
    done
    printf '\r%d von %d Tests fertig, %d Minuten vergangen ...   ' "$DONE" "$TOTAL_TESTS" $(( (SECONDS - START) / 60 ))
done
wait
echo
WALL=$(( SECONDS - START ))

# --- Ergebnisse einsammeln und prüfen ----------------------------------------------------------------------

FAILED=0
for i in $(seq 1 "$WORKERS"); do
    W="$RUN_DIR/w$i"
    R="$W/testbench/emu64-result.txt"
    if [ ! -f "$R" ] || [ "$(cat "$W/exitcode" 2> /dev/null)" != "0" ] || ! tail -n 1 "$R" | grep -q 'elapsed'; then
        echo "Worker $i ist nicht sauber durchgelaufen, siehe $W/log.txt" >&2
        FAILED=1
    fi
done
[ "$FAILED" = 0 ] || die "Das Ergebnis wurde nicht übernommen. Die Arbeitsdateien liegen in $RUN_DIR"

TMP="$RUN_DIR/result.txt"
cat "$RUN_DIR"/w*/testbench/emu64-result.txt | grep -v '^#' | LC_ALL=C sort -t, -k1,1 -k2,2 -s > "$TMP"
echo "# $((WALL / 60)) minutes and $((WALL % 60)) seconds elapsed (wall time, $WORKERS workers)." >> "$TMP"

COUNT_OK=$(awk -F, '!/^#/ && $3 == "ok"' "$TMP" | wc -l)
COUNT_ERROR=$(awk -F, '!/^#/ && $3 == "error"' "$TMP" | wc -l)
COUNT_TIMEOUT=$(awk -F, '!/^#/ && $3 == "timeout"' "$TMP" | wc -l)
COUNT_NOREF=$(awk -F, '!/^#/ && $3 == "noref"' "$TMP" | wc -l)
COUNT_ALL=$(grep -vc '^#' "$TMP")

cp "$TMP" "$OUT_FILE"
cat > "$OUT_META" <<EOF
version=$VERSION
date=$DATE
git_commit=$(git -C "$REPO_DIR" rev-parse HEAD 2> /dev/null || echo unbekannt)
git_dirty=$(git -C "$REPO_DIR" diff --quiet HEAD 2> /dev/null && echo nein || echo ja)
binary=$EMU64_BIN
testbench_flags=$TESTBENCH_FLAGS
workers=$WORKERS
wall_seconds=$WALL
tests_in_list=$TOTAL_TESTS
results=$COUNT_ALL
ok=$COUNT_OK
error=$COUNT_ERROR
timeout=$COUNT_TIMEOUT
noref=$COUNT_NOREF
partial=$PARTIAL
EOF

echo
echo "Ergebnis: $COUNT_ALL Ergebnisse (ok $COUNT_OK, error $COUNT_ERROR, timeout $COUNT_TIMEOUT, noref $COUNT_NOREF)"
echo "Dauer:    $((WALL / 60)) Minuten $((WALL % 60)) Sekunden"
echo "Datei:    $OUT_FILE"
[ "$COUNT_ALL" -lt "$TOTAL_TESTS" ] && echo "Hinweis:  $((TOTAL_TESTS - COUNT_ALL)) Tests der Liste haben kein Ergebnis (interaktive Tests werden übersprungen)."

if [ "$PARTIAL" = 0 ]; then
    "$TESTBENCH_DIR/make_report.sh" || die "Der Bericht konnte nicht erzeugt werden."
fi
