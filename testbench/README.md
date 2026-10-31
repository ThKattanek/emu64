# Testbench-Läufe für Emu64

Dieses Verzeichnis führt die VICE-Testbench (`testprogs/testbench` aus dem VICE-Repo) für Emu64 aus und
sammelt die Ergebnisse jedes Releases in einer HTML-Tabelle, damit man den Fortschritt sieht.

## Benutzung

```bash
cd testbench
./run_release.sh                  # kompletter Lauf, Version aus "git describe", Datum von heute
./run_release.sh -v 5.3.0 -j 8    # Version vorgeben, 8 parallele Worker
./run_release.sh -g drive/scanner # Teillauf, wird NICHT in die Tabelle übernommen
./make_report.sh                  # nur die HTML-Tabellen neu erzeugen
```

Ein kompletter Lauf legt `results/emu64-<Version>-<Datum>-result.txt` und `results/emu64-<Version>-<Datum>.meta`
an und erzeugt danach neu:

- `results/index.html`: nur Tests, die in mindestens einem Release scheitern oder sich zwischen Releases unterscheiden
- `results/index-vollstaendig.html`: alle Tests

Pro Release gibt es eine Spalte (neuestes links), im Kopf stehen die bestandenen Tests je Release.
Die Ergebnisdateien, `.meta` und die HTML-Dateien gehören ins Git. Teilläufe (`results/partial/`) nicht.

Die Dateien `emu64-unbekannt-*` sind die früheren Läufe aus der VICE-Testbench (ohne Versionsangabe).

## Voraussetzungen

- VICE-Testbench ausgecheckt und gebaut (dort einmal `make prereq`). Pfad: `VICE_TESTBENCH` in `config.sh`.
- Installiert: `petcat`, `c1541`, `acme`, `micro64disktool`, `python3`, `rsync`, `cc`.
- Zu testende Binary: `build/install_linux/bin/emu64` (oder `-e <pfad>`, Daten liegen unter `../share`).

Eigene Einstellungen (Pfade, Worker-Anzahl) in `config.local.sh` ablegen oder als Umgebungsvariable setzen.

## Wie der Lauf aufgebaut ist

1. Die VICE `testprogs` werden nach `build/testbench_work/` gespiegelt. Die Tests schreiben auf Disk-Images und
   legen Screenshots ab. Das VICE-Verzeichnis bleibt unverändert, jeder Lauf startet mit sauberen Images.
2. `split_testlist.py` teilt die Testliste auf die Worker auf. Tests im selben Verzeichnis bleiben zusammen,
   weil sie sich Images und Screenshot-Ordner teilen.
3. Jeder Worker hat ein eigenes `testbench`-Verzeichnis (eigene Testliste und Ergebnisdatei) und ein eigenes
   `HOME` (sonst teilen sich alle Prozesse eine `emu64.ini`).
4. Emu64 wird immer mit `--multiple-instance` gestartet. Ohne den Parameter reicht Emu64 die Argumente an eine
   laufende Instanz weiter und beendet sich sofort mit Exitcode 0, die Testbench zeigt dann fälschlich "ok".
5. Nur wenn alle Worker sauber fertig sind, werden die Ergebnisse zusammengeführt und übernommen.

## Hinweise

- Ein Lauf braucht eine laufende Desktop-Sitzung, so wie die bisherigen Läufe auch (Qt).
- `start.sh` ist die alte Variante für einzelne Tests und bleibt unverändert.
- Dasselbe Datum und dieselbe Version zweimal geht nur mit `--force`.
- `maketable` erlaubt im Original nur 16 Spalten. `make_report.sh` übersetzt dafür eine Kopie mit 64 Spalten.
