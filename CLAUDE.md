# CLAUDE.md

Emu64 ist ein Commodore-64-Emulator (C++17, Qt5/Qt6, SDL2, OpenGL) von Thorsten Kattanek. Linux und Windows. Sprache im Projekt (Changelog, Kommentare, Issues, Commits): überwiegend Deutsch, Commit-Messages teils Englisch.

## Build

qmake-Projekt: `emu64.pro` (subdirs) -> `src/src.pro`. Out-of-source in `build/` (git-ignoriert; dort liegen Qt-Creator-Kits und `install_linux/`).

```bash
mkdir -p build/tmp && cd build/tmp
qmake6 ../.. PREFIX=/usr/local   # oder qmake für Qt5
make -j$(nproc)
```

- Abhängigkeiten (pkg-config): sdl2, SDL2_image, libpng, glu, gl, ffmpeg-Libs (avutil, avformat, avcodec, swresample, swscale), quazip (`quazip1-qt5` bzw. Qt6-Variante).
- Version kommt aus `git describe` (Fallback `EMU64_VERSION` in `src/src.pro`).
- CI (`.github/workflows/ci.yml`, Branches `master` und `working_branch`) baut mit gcc und clang und `-O2 -Wall -Wextra -Werror -pedantic -Wshadow=local`. **Neuer Code muss warnungsfrei sein**, sonst schlägt CI fehl.
- Neue Dateien müssen in `src/src.pro` eingetragen werden. Neue UI-Strings über `emu64_de.ts` / `emu64_en.ts` (die `.qm` sind eingecheckt).
- Release-Skripte: `create_linux_appimage.sh --qt /usr/lib/qt6`, `crossbuild-win-releases.sh`.

## Tests

Es gibt keine Unit-Tests. Genauigkeit wird mit der VICE-Testbench geprüft: `testbench/start.sh [testname]` (erwartet das VICE-Repo `testprogs/testbench` und eine installierte Emu64-Binary unter `build/install_linux/bin`; Pfade im Skript sind lokal fest verdrahtet).
Headless-Läufe: `--nogui`, `--autostart <datei>`, `--limitcycles <n>`, `--debugcart` (siehe `kommandozeilenparameter.txt`).
Release-Läufe der kompletten Testbench (parallel, mit Ergebnis-Historie): `testbench/run_release.sh` (siehe `testbench/README.md`). Ergebnisse und `index.html` liegen in `testbench/results/`. **Testläufe immer mit `--multiple-instance` starten** (macht `run_release.sh` selbst): Ohne den Parameter reicht Emu64 die Argumente an eine laufende Instanz weiter und beendet sich sofort mit Exitcode 0, die Testbench zeigt dann fälschlich "ok".

## Architektur (`src/`)

- `c64_class.*`: zentrale Maschine (~5400 Zeilen). Besitzt alle Chips, den Takt-Loop, Audio/Video-Ausgabe, Autostart, Snapshots.
- CPU: `mos6510_class` (C64), `mos6502_class` (Floppy), Microcode-Tabellen `micro_code_tbl_*.h` (zyklusgenau).
- Speicher: `mmu_class` (Banking), `mk7pla.h` (PLA-Logik).
- VIC-II: `mos6569_class`. CIA: `mos6526_class` + `mos6526_port`. VIA (1541): `mos6522_class`.
- SID: eigene Engine `mos6581_8085_*` und reSID (`src/resid/`, Wrapper `resid_wrapper`), wählbar zur Laufzeit.
- Floppy: `floppy1541_class` (bitweise GCR-Emulation), `d64_class`. Band: `tape1530_class`, `t64_class`.
- Erweiterungen: `cartridge_class`, `crt_class`, `am29f040_class` (EasyFlash), `reu_class`, `georam_class`.
- Video: `video_crt_class` (CRT-Shader/Filter), `video_capture_class` (ffmpeg).
- GUI: `main_window` plus viele `*_window` (Debugger, Floppy, Tape, Cartridge, Setup, ...) und Widgets `widget_*`. UI-Dateien als `.ui`.
- Fernsteuerung/Single-Instance: `single_application`, `command_line_class`, `emu64_commands.h`.

## Konventionen

- Klassen heißen `*_class`, Dateien `snake_case`, passend zum umgebenden Code bleiben (deutsche Kommentare/Bezeichner sind üblich).
- Kein `strcpy`; stattdessen `snprintf` (wurde projektweit ersetzt).
- Bei Chip-Änderungen auf Zyklusgenauigkeit achten (CPU/CIA/VIA/VIC/Floppy sind zeitlich gekoppelt). Aktuelle Arbeit (Branch `fixed_timing_problems`) betrifft CIA/VIA-Timing und IEC-Bus.
- Änderungen mit Issue-Bezug im Changelog `änderungen.txt` vermerken (Format: `- Fixed: ...` / `- NEW! [#nnn] ...`).
- `src/resid/` ist Fremdcode (reSID); nur im Notfall anfassen.

## Git

Hauptbranch `master`, Entwicklung auf `working_branch`, PRs gegen diese beiden.

**Nie selbst committen oder pushen.** Der User committet immer selbst. Am Ende einer Änderung nur den Commit-Text ausgeben (kurz, Stil wie im Log, z.B. `Fixed CIA: ...`).
