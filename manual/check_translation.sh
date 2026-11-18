#!/usr/bin/env bash
# Prüft, welche englischen Handbuch-Kapitel älter sind als ihr deutsches Original.
#
# Aufruf: manual/check_translation.sh
# Exitcode: 0 = alles aktuell, 1 = mindestens ein Kapitel veraltet/fehlt
#
# Zeitpunkt einer Datei: Zeitpunkt des letzten Commits, der sie ändert. Ist die Datei
# unversioniert oder hat uncommittete Änderungen, zählt stattdessen die Änderungszeit
# (mtime) der Datei.

cd "$(dirname "${BASH_SOURCE[0]}")" || exit 2

# Zuordnung: deutsche Datei -> englische Datei (bei neuen Kapiteln hier ergänzen)
declare -A MAP=(
    [einfuehrung.adoc]=en/introduction.adoc
    [erste_schritte.adoc]=en/getting_started.adoc
    [menues.adoc]=en/menus.adoc
    [floppy.adoc]=en/floppy.adoc
    [datasette.adoc]=en/datasette.adoc
    [steckmodule.adoc]=en/cartridges.adoc
    [autostart.adoc]=en/autostart.adoc
    [erweiterungen.adoc]=en/expansions.adoc
    [snapshots.adoc]=en/snapshots.adoc
    [einstellungen.adoc]=en/settings.adoc
    [tastatur.adoc]=en/keyboard.adoc
    [tools.adoc]=en/tools.adoc
    [kommandozeile.adoc]=en/commandline.adoc
    [debugger.adoc]=en/debugger.adoc
    [genauigkeit.adoc]=en/accuracy.adoc
    [aufbau.adoc]=en/architecture.adoc
    [dateiformate.adoc]=en/file_formats.adoc
    [anhang.adoc]=en/appendix.adoc
)

file_time() {
    local f=$1 t=""
    if [ -z "$(git status --porcelain -- "$f" 2>/dev/null)" ]; then
        t=$(git log -1 --format=%ct -- "$f" 2>/dev/null)
    fi
    [ -n "$t" ] || t=$(stat -c %Y "$f")
    echo "$t"
}

fmt() { date -d "@$1" '+%Y-%m-%d %H:%M'; }

bad=0
out=""
for de in "${!MAP[@]}"; do
    en=${MAP[$de]}
    if [ ! -f "$de" ]; then
        out+="FEHLT (deutsch):  $de"$'\n'; bad=1; continue
    fi
    if [ ! -f "$en" ]; then
        out+="FEHLT (englisch): $en  (Original: $de)"$'\n'; bad=1; continue
    fi
    tde=$(file_time "$de"); ten=$(file_time "$en")
    if [ "$tde" -gt "$ten" ]; then
        out+="VERALTET:         $en  (en: $(fmt "$ten"), $de: $(fmt "$tde"))"$'\n'
        bad=1
    else
        out+="ok:               $en"$'\n'
    fi
done
printf '%s' "$out" | sort

# Deutsche Kapitel, die in index.adoc eingebunden, aber nicht in MAP stehen
for f in $(grep -oP '^include::\K[^[]+(?=\[)' index.adoc | grep -v '^settings.adoc$'); do
    [ -n "${MAP[$f]}" ] || { echo "NICHT ZUGEORDNET: $f (in MAP des Skripts eintragen)"; bad=1; }
done

exit $bad
