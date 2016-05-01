Verzeichnisstruktur:

MXE Verzeichnis:	  /media/daten/mxe
Emu64 Projektverzeichnis: ~/Programmierung/Aktuelle_Projekte/emu64
Build Verzeichnis: 	  ~/Programmierung/Aktuelle_Projekte/builds

Linux:

#32Bit
cd ~/Programmierung/Aktuelle_Projekte/builds
mkdir emu64-linux-i686
qmake ../../emu64/emu64_projekt.pro -spec linux-g++-32 -r
make -j8

#64Bit
cd ~/Programmierung/Aktuelle_Projekte/builds
mkdir emu64-linux-amd64
qmake ../../emu64/emu64_projekt.pro -spec linux-g++-64 -r
make -j8

Windows Cross (mit MXE):

cd ~/Programmierung/Aktuelle_Projekte/builds

#64Bit Version Shared
../emu64/cross_build.sh /media/daten/mxe x86_64 shared ~/Programmierung/Aktuelle_Projekte/emu64/emu64_projekt.pro

#32Bit Version Shared
../emu64/cross_build.sh /media/daten/mxe i686 shared ~/Programmierung/Aktuelle_Projekte/emu64/emu64_projekt.pro

Paketbau Linux Debian:

build_deb.sh nach ~/Programmierung/Aktuelle_Projekte/builds/emu64-linux-amd64 kopieren

cd ~/Programmierung/Aktuelle_Projekte/builds/emu64-linux-amd64
build_deb.sh anpassen --> Version usw.
./build_deb.sh

