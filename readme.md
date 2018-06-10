# EMU64 - Der C64 Emulator
![Logo](http://www.emu64-projekt.de/images/emu64_logo_120_120.png)

Emu64 Homepage: [http://www.emu64-projekt.de](http://www.emu64-projekt.de)

Emu64 auf YouTube: [Emu64 News Playlist](https://www.youtube.com/playlist?list=PLPygkia21sCKyHtZ9DGkWhHrq3bF9fMhY) / [Emu64 Tutorial Playlist](https://www.youtube.com/playlist?list=PLPygkia21sCLN7UtYWqpuGRjmC6OTV8mY)

Emu64 auf Facebook: [https://www.facebook.com/Emu64-103321833093172](https://www.facebook.com/Emu64-103321833093172)

## Screenshot
![Screenshot](screenshot.png)


## Emu64 erstellen unter Linux

### Benötigte Dev Pakete 
- qt4
- git
- cmake
- sdl2
- sdl2-image
- quazip
- png
- ffmpeg
  - avcodec
  - avutil
  - swresample
  - avformat
  - swscale

### Build
```bash
git clone https://github.com/ThKattanek/emu64.git
cd emu64
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make install
```
Optional kann noch -DCMAKE_INSTALL_PREFIX=[InstallPfad] verwendet werden.

### Deinstallieren
```bash
xargs rm < install_manifest.txt
```
Achtung! Verzeichnisse die durch die Installation erstellt wurden werden hiermit nicht entfernt, jedoch alle Dateien die erstellt wurden.

### Detailierte Beschreibungen für verschiedene Linux Distributionen

[Kompilieren unter Fedora 27](https://github.com/ThKattanek/emu64/wiki/Emu64-auf-Fedora-27-kompilieren)

[Kompilieren unter Debian 9.3.0](https://github.com/ThKattanek/emu64/wiki/Emu64-auf-Debian-9.3.0-kompilieren)

[Windows Versionen unter Linux mit MXE](https://github.com/ThKattanek/emu64/wiki/Windows-Build-unter-Linux-mit-MXE-erstellen)

