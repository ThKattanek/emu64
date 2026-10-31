# Testbench Guide / Anleitung

[Deutsch](#deutsch) | [English](#english)

---

## Deutsch

Mit diesen Skripten läuft die VICE-Testbench für Emu64 parallel durch. Die Ergebnisse jedes Releases landen in einer HTML-Tabelle, so sieht man den Fortschritt von Release zu Release.

### Einmal vorbereiten

1. VICE-Testbench ist ausgecheckt und gebaut (dort einmal `make prereq`).
2. Installiert sind `petcat`, `c1541`, `acme`, `micro64disktool`, `python3`, `rsync` und ein C-Compiler.
3. Emu64 ist gebaut und installiert unter `build/install_linux/bin/emu64`.
4. Liegt die VICE-Testbench woanders, den Pfad in `testbench/config.local.sh` eintragen (wird nicht eingecheckt):

   ```bash
   VICE_TESTBENCH="/pfad/zu/vice/testprogs/testbench"
   ```

### Für ein Release ausführen

```bash
cd testbench
./run_release.sh -v 5.3.0
```

- Ohne `-v` wird die Version aus `git describe` genommen. Das Datum ist das von heute.
- Die Dauer hängt von der Zahl der Worker ab. Der letzte Lauf ohne Parallelisierung brauchte rund 80 Minuten. Der Fortschritt wird angezeigt.
- Mit `-j 8` bestimmst du die Zahl der parallelen Worker (Standard: halbe Kernzahl, höchstens 12).
- Während des Laufs kannst du Emu64 normal weiter benutzen, die Tests laufen in eigenen Prozessen.

### Ergebnis ansehen

Öffne `testbench/results/index.html` im Browser.

- Eine Spalte pro Release, das neueste links.
- Im Kopf steht je Release, wie viele Tests bestanden sind.
- `index.html` zeigt nur Tests, die scheitern oder sich zwischen Releases unterscheiden. `index-vollstaendig.html` zeigt alle.

### Danach committen

Ins Git gehören die neue `emu64-<Version>-<Datum>-result.txt`, die zugehörige `.meta` und die beiden HTML-Dateien. Die Skripte committen nichts.

### Nützliches

| Befehl | Wirkung |
|---|---|
| `./run_release.sh -g drive/scanner` | Teillauf nur für passende Verzeichnisse. Er wird nicht in die Tabelle übernommen. |
| `./run_release.sh -e /pfad/emu64` | Eine andere Binary testen. |
| `./run_release.sh --force` | Ein vorhandenes Ergebnis mit gleichem Namen überschreiben. |
| `./make_report.sh` | Nur die HTML-Tabellen neu erzeugen. |

### Wenn etwas nicht klappt

- **Alle Tests sind sofort "ok":** Es wurde eine laufende Emu64-Instanz angesprochen und nichts getestet. Die Skripte starten Emu64 deshalb immer mit `--multiple-instance`. Bei eigenen Aufrufen den Parameter selbst angeben.
- **Ein Worker bricht ab:** Das Ergebnis wird nicht übernommen. Das Log steht in `build/testbench_work/run-<Name>/w<N>/log.txt`.
- **Fehlermeldung zu fehlenden Programmen:** Die Voraussetzungen oben prüfen.

Weitere Details stehen in `README.md`.

---

## English

These scripts run the VICE test bench for Emu64 in parallel. The results of every release end up in one HTML table, so you can see the progress from release to release.

### One-time setup

1. The VICE test bench is checked out and built (run `make prereq` there once).
2. `petcat`, `c1541`, `acme`, `micro64disktool`, `python3`, `rsync` and a C compiler are installed.
3. Emu64 is built and installed at `build/install_linux/bin/emu64`.
4. If the VICE test bench is somewhere else, put its path into `testbench/config.local.sh` (not checked in):

   ```bash
   VICE_TESTBENCH="/path/to/vice/testprogs/testbench"
   ```

### Run it for a release

```bash
cd testbench
./run_release.sh -v 5.3.0
```

- Without `-v` the version is taken from `git describe`. The date is today's date.
- The duration depends on the number of workers. The last run without parallelism took about 80 minutes. Progress is shown while it runs.
- `-j 8` sets the number of parallel workers (default: half the cores, at most 12).
- You can keep using Emu64 normally during a run, the tests run in their own processes.

### Look at the result

Open `testbench/results/index.html` in a browser.

- One column per release, the newest on the left.
- The header shows how many tests each release passes.
- `index.html` shows only tests that fail or differ between releases. `index-vollstaendig.html` shows all of them.

### Commit afterwards

Commit the new `emu64-<version>-<date>-result.txt`, its `.meta` file and the two HTML files. The scripts do not commit anything.

### Useful options

| Command | Effect |
|---|---|
| `./run_release.sh -g drive/scanner` | Partial run for matching directories only. It is not added to the table. |
| `./run_release.sh -e /path/emu64` | Test a different binary. |
| `./run_release.sh --force` | Overwrite an existing result with the same name. |
| `./make_report.sh` | Only rebuild the HTML tables. |

### If something goes wrong

- **Every test is "ok" immediately:** A running Emu64 instance received the arguments and nothing was tested. The scripts always start Emu64 with `--multiple-instance` for this reason. If you call Emu64 yourself, pass the parameter yourself.
- **A worker aborts:** The result is not stored. The log is in `build/testbench_work/run-<name>/w<N>/log.txt`.
- **Error about missing programs:** Check the setup list above.

More details are in `README.md`.
