#!/bin/bash
# Erzeugt aus allen Ergebnisdateien results/emu64-<Version>-<Datum>-result.txt die HTML Tabellen:
#
#   results/index.html               nur Tests, die in mindestens einem Release scheitern oder sich zwischen
#                                    den Releases unterscheiden
#   results/index-vollstaendig.html  alle Tests
#
# Pro Release gibt es eine Spalte (neuestes links). Am Kopf steht je Release die Zahl der bestandenen Tests,
# daran sieht man den Fortschritt. Neue Releases werden einfach als weitere Ergebnisdatei angehängt:
# das Skript liest immer alle Dateien neu ein. Wird von run_release.sh am Ende automatisch aufgerufen.
#
# Die Tabelle wird mit "maketable" aus der VICE Testbench erzeugt. Es wird eine Kopie mit mehr Spalten
# übersetzt (das Original erlaubt nur 16), das VICE Verzeichnis bleibt unverändert.

set -u

source "$(dirname "${BASH_SOURCE[0]}")/config.sh"

die() { echo "Fehler: $*" >&2; exit 1; }

[ -f "$VICE_TESTBENCH/maketable.c" ] || die "maketable.c nicht gefunden in $VICE_TESTBENCH"
[ -f "$VICE_TESTBENCH/emu64-testlist.txt" ] || die "emu64-testlist.txt nicht gefunden in $VICE_TESTBENCH"
command -v cc > /dev/null || die "Ein C Compiler (cc) wird gebraucht."

# --- maketable bereitstellen -------------------------------------------------------------------------------

BIN_DIR="$WORK_DIR/bin"
MAKETABLE="$BIN_DIR/maketable"
mkdir -p "$BIN_DIR"
if [ ! -x "$MAKETABLE" ] || [ "$VICE_TESTBENCH/maketable.c" -nt "$MAKETABLE" ]; then
    sed 's/^#define MAXLISTS .*/#define MAXLISTS    0x40/' "$VICE_TESTBENCH/maketable.c" > "$BIN_DIR/maketable.c"
    cc -W -Wall -O3 -o "$MAKETABLE" "$BIN_DIR/maketable.c" -lm || die "maketable konnte nicht übersetzt werden"
fi

# --- Ergebnisdateien sammeln: neueste zuerst ---------------------------------------------------------------

mapfile -t FILES < <(
    for f in "$RESULTS_DIR"/emu64-*-result.txt; do
        [ -f "$f" ] || continue
        if [[ "$(basename "$f")" =~ -([0-9]{8})-result\.txt$ ]]; then
            echo "${BASH_REMATCH[1]} $f"
        fi
    done | LC_ALL=C sort -r | cut -d' ' -f2-
)
[ ${#FILES[@]} -gt 0 ] || die "Keine Ergebnisdateien in $RESULTS_DIR gefunden."

ARGS=()

# Optionale Referenzspalte: neuestes x64sc Ergebnis der VICE Testbench (zeigt Tests, die auch in VICE scheitern)
REF="$(ls "$VICE_TESTBENCH"/results/x64sc-*-result.txt 2> /dev/null | sort -V | tail -n 1)"
if [ -n "$REF" ]; then
    REF_NAME="$(basename "$REF" -result.txt)"
    ARGS+=(--results "$REF" "x64sc ${REF_NAME#x64sc-}" --firstcolisref "<small><b>fails on VICE</b></small>")
fi

for f in "${FILES[@]}"; do
    base="$(basename "$f" -result.txt)"          # emu64-<Version>-<Datum>
    rest="${base#emu64-}"
    date="${rest##*-}"
    version="${rest%-*}"
    pretty="${date:0:4}-${date:4:2}-${date:6:2}"
    if [ "$version" = "unbekannt" ]; then
        header="emu64<br><small>$pretty</small>"
    else
        header="emu64 $version<br><small>$pretty</small>"
    fi
    ARGS+=(--results "$f" "$header")
done

COMMON=(--html --list "$VICE_TESTBENCH/emu64-testlist.txt"
        --ranking "<small><hr>%d%%<br>%d of %d<small>"
        --percentages "<small><hr>%d%%<br>%d of %d<small>")

make_one()
{
    local out="$1"; shift
    "$MAKETABLE" "$@" "${COMMON[@]}" "${ARGS[@]}" > "$out.tmp" || die "maketable ist fehlgeschlagen ($out)"
    mv "$out.tmp" "$out"
    echo "Geschrieben: $out"
}

make_one "$RESULTS_DIR/index.html" --omit-clean
make_one "$RESULTS_DIR/index-vollstaendig.html"
