# EMU64 - Der C64 Emulator
![Logo](grafik/emu64_logo_120_120.png)

**EMU64** ist ein Emulator für den Commodore 64, einen legendären Home-Computer aus den 1980er Jahren. Das Projekt bietet authentische Emulation mit umfangreichen Features für Retro-Gaming und Softwareentwicklung.

### [Letzte offizielle Linux & Windows Version](https://github.com/ThKattanek/emu64/releases/latest)
(Die Windows Version (32 + 64Bit) läuft auch unter Linux mit Wine - getestet unter Kubuntu 26.04 LTS)

## Inhaltsverzeichnis
- [Features](#features)
- [Links](#links)
- [Screenshot](#screenshot)
- [Installation](#emu64-erstellen-unter-linux)
- [Lizenz](#lizenz)

## Features
- Authentischer, zyklusgenauer Commodore 64 Emulator (6510, VIC-II 6569/8565, CIA 6526/6526A, SID, 1541)
- Multi-Plattform Support (Windows, Linux)
- SDL2-basierte Audio- und Video-Ausgabe, OpenGL mit CRT-Emulation (Shader/Filter)
- SID wahlweise mit eigener Engine oder reSID (zur Laufzeit wählbar)
- Bitgenaue 1541-Floppyemulation (D64 mit 35/40/42 Spuren, G64, NIB (nur lesend), Fastloader, schwache Bits), Datasette (TAP, T64)
- Steckmodule (CRT, u.a. EasyFlash), REU und GeoRAM
- Eingebauter Debugger mit zyklusgenauer Einzelschrittanzeige des Bildaufbaus
- Warp-Modus, Videoaufnahme (ffmpeg), Screenshots, Snapshots
- Kommandozeilenparameter für Autostart und Headless-Läufe (`--nogui`, `--autostart`, `--limitcycles`, ...), siehe [kommandozeilenparameter.txt](kommandozeilenparameter.txt)
- Tastenbelegung siehe [hotkeys.txt](hotkeys.txt), Änderungen siehe [änderungen.txt](änderungen.txt)
- Mehrsprachige Oberfläche (Deutsch, Englisch)
- Qt6 oder Qt5 GUI

## Links
**Homepage:** [https://www.thorsten-kattanek.de/index.php/projekte/emu64](https://www.thorsten-kattanek.de/index.php/projekte/emu64)

**YouTube:** [Emu64 News Playlist](https://www.youtube.com/playlist?list=PLPygkia21sCKyHtZ9DGkWhHrq3bF9fMhY) | [Emu64 Tutorial Playlist](https://www.youtube.com/playlist?list=PLPygkia21sCLN7UtYWqpuGRjmC6OTV8mY)

**Facebook:** [https://www.facebook.com/Emu64-103321833093172](https://www.facebook.com/Emu64-103321833093172)

## Screenshot
![Screenshot](screenshot.png)

## Installation

### Emu64 erstellen unter Linux

#### Wichtige Voraussetzungen
- C++17-Compiler (gcc oder clang), make, pkg-config
- qttools6-dev (Qt6) oder Qt5 inkl. Qt-Tools

#### Erforderliche Bibliotheken
- qt6 oder qt5
- sdl2
- sdl2-image
- libpng
- OpenGL und GLU (`gl`, `glu`)
- quazip (`quazip1-qt5` bzw. die Qt6-Variante)
- ffmpeg (mit: avcodec, avutil, swresample, avformat, swscale)

#### Build- und Installationsanleitung
```bash
git clone https://github.com/ThKattanek/emu64.git
cd emu64
# Optional: Eine spezifische Release-Version verwenden (z.B. 5.2.0)
# git checkout 5.2.0
# Die Version wird aus `git describe` ermittelt
mkdir -p build/tmp
cd build/tmp
qmake6 ../.. PREFIX="/usr/local"
make -j$(nproc)
sudo make install
```

Für Qt5 statt `qmake6` einfach `qmake` verwenden. Das Verzeichnis `build/` wird von Git ignoriert.

#### Übersetzung aktualisieren
Um die Übersetzung auf andere Sprachen zu aktualisieren, müssen die `*.ts`-Dateien aktualisiert werden:

```bash
lupdate emu64.pro
# oder
lupdate src/src.pro
```

Danach können Sie mit dem QLinguist-Tool die entsprechende `*.ts`-Datei laden und neue Wörter übersetzen. Nach dem Speichern werden die `qm`-Dateien automatisch beim Kompilieren erzeugt.

#### Fehlerbehebung: "make install"
Falls dieser Fehler auftritt:
```
Aufruf von stat für '.qm/emu64_de.qm' nicht möglich: Datei oder Verzeichnis nicht gefunden
```

Führen Sie folgende Befehle aus:
```bash
mkdir src/.qm
lrelease ../src/src.pro
cp ../src/*.qm src/.qm
sudo make install
```

#### Deinstallation
```bash
xargs rm < install_manifest.txt
```

**Hinweis:** Verzeichnisse, die während der Installation erstellt wurden, werden nicht entfernt, aber alle Dateien werden gelöscht.

#### Tests
Die Emulationsgenauigkeit wird mit der VICE-Testbench geprüft, siehe [testbench/README.md](testbench/README.md).

#### Weitere Informationen
Detaillierte Anleitungen für ein Windows-Build mit MXE:
- [Windows-Versionen unter Linux mit MXE erstellen](https://github.com/ThKattanek/emu64/wiki/Windows-Build-unter-Linux-mit-MXE-erstellen)

## Lizenz
Dieses Projekt ist unter der GNU General Public License v2 (GPLv2) lizenziert. Siehe [LICENSE](LICENSE) für Details.