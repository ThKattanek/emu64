![Logo](http://www.emu64-projekt.de/images/emu64_logo_120_120.png)

## Emu64 erstellen unter Linux

### Benötigte Pakete 
- qt-sdk
- git
- cmake
- libsdl2-dev
- libsdl2-image-dev
- libquazip-dev
- libpng-dev
- libavcodec-dev
- libavutil-dev
- libswresample-dev
- libavformat-dev
- libswscale-dev

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
Achtung! Erstellte Verzeichnisse die durch die Installation erstellt wurden werden hiermit nicht entfernt, jedoch alle Dateien die erstellt wurden.

### Detailierte Beschreibungen für verschiedene Linux Distributionen

[Kompilieren unter Fedora 27](https://github.com/ThKattanek/emu64/wiki/Emu64-auf-Fedora-27-kompilieren)
[Kompilieren unter Debian 3.9.9](https://github.com/ThKattanek/emu64/wiki/Emu64-auf-Debian-9.3.0-kompilieren)

