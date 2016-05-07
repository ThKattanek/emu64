Verzeichnisstruktur:

MXE Verzeichnis:	  /media/daten/mxe
Emu64 Projektverzeichnis: ~/Programmierung/Aktuelle_Projekte/emu64
Build Verzeichnis: 	  ~/Programmierung/Aktuelle_Projekte/builds

Linux:

#32Bit
cd ~/Programmierung/Aktuelle_Projekte/builds
mkdir emu64-linux-i686
cd emu64-linux-i686
qmake ../../emu64/emu64_projekt.pro -spec linux-g++-32 -r
make -j8

#64Bit
cd ~/Programmierung/Aktuelle_Projekte/builds
mkdir emu64-linux-amd64
emu64-linux-amd64
qmake ../../emu64/emu64_projekt.pro -spec linux-g++-64 -r
make -j8

Windows Cross (mit MXE):

cd ~/Programmierung/Aktuelle_Projekte/builds

#64Bit Version Shared
../emu64/cross_build.sh /media/daten/mxe x86_64 shared ~/Programmierung/Aktuelle_Projekte/emu64/emu64_projekt.pro

#32Bit Version Shared
../emu64/cross_build.sh /media/daten/mxe i686 shared ~/Programmierung/Aktuelle_Projekte/emu64/emu64_projekt.pro

Paketbau Linux Debian / Red Hat:

Vorraussetzung --> alien muss installiert sein

build_deb.sh nach ~/Programmierung/Aktuelle_Projekte/builds/emu64-linux-amd64 kopieren

cd ~/Programmierung/Aktuelle_Projekte/builds/emu64-linux-amd64
build_deb.sh anpassen --> Version usw.
./build_deb.sh

--> Es befindet sich nun ein deb und rpm im Verzeichnis

Die deb sind fertig aber die rpm müssen noch mal mittels rpmrebuild angepasst werden.
Das geschieht in Fedora Version 23 32-Bit und 64-32Bit

rpmrebuild -pe emu64-5.0.10-1.i386.rpm 

jetzt prüfen ob die Rechte der Verzeichnisse und Dateien stimmt (bei i386 stimmen die eigtl. nicht)
    
    Alle Verzeichnisse und /usr/bin/emu64 --> 0755
    Alle anderen Dateien --> 0644
    
Und dann müssen "/" und "/usr/bin" entfernt werden, sonst kommt ein Fehler beim installieren.

(Das Editieren funktioniert halt wie unter vi (a -> Editieren esc -> Editmode verlassen :wq -> schreiben und verlassen))

Mit Y bestätigen und das geänderte rpm wird im Homeverzeichnis gespeichert !!
