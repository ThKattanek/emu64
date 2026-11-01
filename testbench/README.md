# Emu64 Testbench

[Deutsch](#deutsch) | [English](#english)

---

## Deutsch

Dieses Verzeichnis führt die VICE-Testbench (`testprogs/testbench` aus dem VICE-Repo) für Emu64 parallel aus und sammelt die Ergebnisse jedes Releases in einer HTML-Tabelle. So sieht man den Fortschritt von Release zu Release.

### Einmal vorbereiten

1. VICE-Testbench ist ausgecheckt und gebaut (dort einmal `make prereq`).
2. Installiert sind `petcat`, `c1541`, `acme`, `micro64disktool`, `python3`, `rsync` und ein C-Compiler.
3. Emu64 ist gebaut und installiert unter `build/install_linux/bin/emu64` (die Daten liegen daneben unter `../share`).
4. Liegt die VICE-Testbench woanders, den Pfad in `testbench/config.local.sh` eintragen (wird nicht eingecheckt). Dort lassen sich auch die Worker-Anzahl und weitere Werte aus `config.sh` überschreiben:

   ```bash
   VICE_TESTBENCH="/pfad/zu/vice/testprogs/testbench"
   ```

### Für ein Release ausführen

```bash
cd testbench
./run_release.sh -v 5.3.0
```

- Ohne `-v` wird die Version aus `git describe` genommen. Das Datum ist das von heute.
- Die Dauer hängt von der Zahl der Worker ab. Der letzte Lauf ohne Parallelisierung brauchte rund 80 Minuten, der erste parallele Lauf mit 12 Workern 28 Minuten. Der Fortschritt wird angezeigt.
- Mit `-j 8` bestimmst du die Zahl der parallelen Worker (Standard: halbe Kernzahl, höchstens 12).
- Ein Lauf braucht eine Desktop-Sitzung (Qt), so wie die bisherigen Läufe auch. Emu64 lässt sich währenddessen normal weiter benutzen, die Tests laufen in eigenen Prozessen.

### Ergebnis ansehen

Öffne `testbench/results/index.html` im Browser.

- Eine Spalte pro Release, das neueste links. Die Reihenfolge bestimmt das Datum im Dateinamen.
- Im Kopf steht je Release, wie viele Tests bestanden sind.
- `index.html` zeigt nur Tests, die scheitern oder sich zwischen Releases unterscheiden. `index-vollstaendig.html` zeigt alle.
- Die Dateien `emu64-unbekannt-*` sind die früheren Läufe aus der VICE-Testbench (ohne Versionsangabe).

### Danach committen

Ins Git gehören die neue `results/emu64-<Version>-<Datum>-result.txt`, die zugehörige `.meta` und die beiden HTML-Dateien. Die Skripte committen nichts. Teilläufe (`results/partial/`) gehören nicht ins Git.

### Eine ältere Version nachtragen

Die Testskripte gibt es nur im aktuellen Stand, nicht in einem alten Tag. Deshalb den alten Stand **nicht** im Hauptverzeichnis auschecken, sondern als zweites Arbeitsverzeichnis. Alle Befehle im Hauptverzeichnis des Repositories (dort, wo `emu64.pro` liegt) starten:

```bash
git worktree add build/release-5.2.0 5.2.0                       # alter Stand, nur zum Bauen
cd build/release-5.2.0 && mkdir -p build/tmp && cd build/tmp
qmake6 ../.. PREFIX=$PWD/../install_linux && make -j$(nproc) && make install
cd <hauptverzeichnis>/testbench
./run_release.sh -v 5.2.0 \
    -e ../build/release-5.2.0/build/install_linux/bin/emu64 \
    -s ../build/release-5.2.0
cd <hauptverzeichnis> && git worktree remove build/release-5.2.0 # wenn fertig
```

- `-s` ist der Git-Stand, aus dem die Binary gebaut wurde. Daraus kommen Commit und Datum (Datum des Tags). Das Datum bestimmt die Reihenfolge der Spalten. Mit `-d JJJJMMTT` lässt es sich überschreiben.
- Die Binary braucht ihr `share`-Verzeichnis daneben (`install_linux/share`), deshalb `make install`.
- Ältere Versionen kennen eventuell nicht alle Parameter der Testbench (zum Beispiel `--set-vicmodel`, `--nosound`). Die betroffenen Tests können dort scheitern, ohne dass es ein echter Fehler ist.

### Nützliche Optionen

| Befehl | Wirkung |
|---|---|
| `./run_release.sh -g drive/scanner` | Teillauf nur für Verzeichnisse, deren Pfad den Text enthält. Er wird nicht in die Tabelle übernommen. |
| `./run_release.sh -e <binary> -s <git-verzeichnis>` | Eine andere Binary testen (bei älteren Versionen zusammen mit `-v`). |
| `./run_release.sh --force` | Ein vorhandenes Ergebnis mit gleichem Namen überschreiben. |
| `./run_release.sh --no-sync` | Die Kopie der VICE-Testprogramme nicht aktualisieren (schneller beim Wiederholen). |
| `./make_report.sh` | Nur die HTML-Tabellen neu erzeugen. |

### Wenn etwas nicht klappt

- **Alle Tests sind sofort "ok":** Eine laufende Emu64-Instanz hat die Argumente übernommen, getestet wurde nichts. Ohne `--multiple-instance` reicht Emu64 die Parameter weiter und beendet sich sofort mit Exitcode 0. Die Skripte setzen den Parameter deshalb immer. Bei eigenen Aufrufen den Parameter selbst angeben.
- **Alle Tests sind sofort "error":** Die Binary startet nicht. Das Log eines Workers steht in `build/testbench_work/run-<Name>/w<N>/log.txt`. Gleich eine Binary von Hand mit `--nogui --limitcycles 1000000` probieren.
- **Ein Worker bricht ab:** Das Ergebnis wird nicht übernommen, die Arbeitsdateien bleiben im Ordner `run-<Name>` zur Fehlersuche.
- **Fehlermeldung zu fehlenden Programmen oder Dateien:** Die Voraussetzungen oben prüfen.

### Wie der Lauf aufgebaut ist

1. Die VICE `testprogs` werden nach `build/testbench_work/` gespiegelt. Die Tests schreiben auf Disk-Images und legen Screenshots ab. Das VICE-Verzeichnis bleibt unverändert, jeder Lauf startet mit sauberen Images.
2. `split_testlist.py` teilt die Testliste auf die Worker auf. Tests im selben Verzeichnis bleiben zusammen, weil sie sich Images und Screenshot-Ordner teilen.
3. Jeder Worker hat ein eigenes `testbench`-Verzeichnis (eigene Testliste und Ergebnisdatei) und ein eigenes `HOME` (sonst teilen sich alle Prozesse eine `emu64.ini`).
4. Emu64 wird immer mit `--multiple-instance` gestartet.
5. Nur wenn alle Worker sauber fertig sind, werden die Ergebnisse zusammengeführt und übernommen.
6. `make_report.sh` erzeugt aus allen `results/emu64-*-result.txt` die Tabellen mit dem Tool `maketable` der VICE-Testbench. Das Original erlaubt nur 16 Spalten, deshalb wird eine Kopie mit 64 Spalten übersetzt.

`start.sh` ist die alte Variante für einzelne Tests und bleibt unverändert.

---

## English

This directory runs the VICE test bench (`testprogs/testbench` from the VICE repo) for Emu64 in parallel and collects the results of every release in one HTML table, so you can see the progress from release to release.

### One-time setup

1. The VICE test bench is checked out and built (run `make prereq` there once).
2. `petcat`, `c1541`, `acme`, `micro64disktool`, `python3`, `rsync` and a C compiler are installed.
3. Emu64 is built and installed at `build/install_linux/bin/emu64` (its data lives next to it in `../share`).
4. If the VICE test bench is somewhere else, put its path into `testbench/config.local.sh` (not checked in). The number of workers and other values from `config.sh` can be overridden there too:

   ```bash
   VICE_TESTBENCH="/path/to/vice/testprogs/testbench"
   ```

### Run it for a release

```bash
cd testbench
./run_release.sh -v 5.3.0
```

- Without `-v` the version is taken from `git describe`. The date is today's date.
- The duration depends on the number of workers. The last run without parallelism took about 80 minutes, the first parallel run with 12 workers took 28 minutes. Progress is shown while it runs.
- `-j 8` sets the number of parallel workers (default: half the cores, at most 12).
- A run needs a desktop session (Qt), like the earlier runs did. You can keep using Emu64 normally meanwhile, the tests run in their own processes.

### Look at the result

Open `testbench/results/index.html` in a browser.

- One column per release, the newest on the left. The order is decided by the date in the file name.
- The header shows how many tests each release passes.
- `index.html` shows only tests that fail or differ between releases. `index-vollstaendig.html` shows all of them.
- The files `emu64-unbekannt-*` are the earlier runs from the VICE test bench (without a version).

### Commit afterwards

Commit the new `results/emu64-<version>-<date>-result.txt`, its `.meta` file and the two HTML files. The scripts do not commit anything. Partial runs (`results/partial/`) do not belong in git.

### Add an older version

The test scripts only exist in the current state, not in an old tag. So do **not** check out the old state in the main directory. Use a second working directory. Start all commands in the main directory of the repository (where `emu64.pro` is):

```bash
git worktree add build/release-5.2.0 5.2.0                       # old state, only for building
cd build/release-5.2.0 && mkdir -p build/tmp && cd build/tmp
qmake6 ../.. PREFIX=$PWD/../install_linux && make -j$(nproc) && make install
cd <main-directory>/testbench
./run_release.sh -v 5.2.0 \
    -e ../build/release-5.2.0/build/install_linux/bin/emu64 \
    -s ../build/release-5.2.0
cd <main-directory> && git worktree remove build/release-5.2.0   # when done
```

- `-s` is the git state the binary was built from. Commit and date (the tag's date) come from there. The date decides the column order. `-d YYYYMMDD` overrides it.
- The binary needs its `share` directory next to it (`install_linux/share`), hence `make install`.
- Older versions may not know every parameter the test bench uses (for example `--set-vicmodel`, `--nosound`). The affected tests may fail there without being a real bug.

### Useful options

| Command | Effect |
|---|---|
| `./run_release.sh -g drive/scanner` | Partial run for directories whose path contains the text. It is not added to the table. |
| `./run_release.sh -e <binary> -s <git-directory>` | Test a different binary (together with `-v` for older versions). |
| `./run_release.sh --force` | Overwrite an existing result with the same name. |
| `./run_release.sh --no-sync` | Do not update the copy of the VICE test programs (faster when repeating). |
| `./make_report.sh` | Only rebuild the HTML tables. |

### If something goes wrong

- **Every test is "ok" immediately:** A running Emu64 instance took over the arguments and nothing was tested. Without `--multiple-instance` Emu64 forwards its parameters and exits immediately with exit code 0. The scripts always pass the parameter. If you call Emu64 yourself, pass it yourself.
- **Every test is "error" immediately:** The binary does not start. The log of a worker is in `build/testbench_work/run-<name>/w<N>/log.txt`. Try the binary by hand with `--nogui --limitcycles 1000000`.
- **A worker aborts:** The result is not stored, the working files stay in the `run-<name>` folder for debugging.
- **Error about missing programs or files:** Check the setup list above.

### How a run is built

1. The VICE `testprogs` are mirrored to `build/testbench_work/`. The tests write to disk images and create screenshots. The VICE directory stays unchanged, every run starts with clean images.
2. `split_testlist.py` distributes the test list over the workers. Tests in the same directory stay together because they share images and screenshot folders.
3. Each worker has its own `testbench` directory (own test list and result file) and its own `HOME` (otherwise all processes share one `emu64.ini`).
4. Emu64 is always started with `--multiple-instance`.
5. Only if all workers finish cleanly are the results merged and stored.
6. `make_report.sh` builds the tables from all `results/emu64-*-result.txt` with the `maketable` tool of the VICE test bench. The original allows only 16 columns, so a copy with 64 columns is compiled.

`start.sh` is the old way to run single tests and stays unchanged.
