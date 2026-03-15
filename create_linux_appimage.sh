# //////////////////////////////////////////////////
# //                                              //
# // Emu64                                        //
# // von Thorsten Kattanek                        //
# //                                              //
# // #file: create_linux_appimage.sh              //
# //                                              //
# // Dieser Sourcecode ist Copyright geschützt!   //
# // Geistiges Eigentum von Th.Kattanek           //
# //                                              //
# // Letzte Änderung am 15.03.2026                //
# // www.emu64-projekt.de                         //
# //                                              //
# //////////////////////////////////////////////////

#!/bin/bash
set -euo pipefail

usage() {
  echo "Usage: $0 --qt <QT_ROOT>" >&2
  echo "  Beispiel: $0 --qt \"$HOME/Qt/6.6.2/gcc_64\"" >&2
  echo "  oder: QT_ROOT=\"$HOME/Qt/6.6.2/gcc_64\" $0" >&2
}

QT_ROOT="${QT_ROOT:-}"

# --qt <path> aus Argumenten lesen (optional)
if [[ ${1:-} == "--qt" ]]; then
  QT_ROOT="${2:-}"
  shift 2 || true
elif [[ ${1:-} == "-h" || ${1:-} == "--help" ]]; then
  usage
  exit 0
fi

if [[ -z "$QT_ROOT" ]]; then
  echo "WARNUNG: Kein QT_ROOT angegeben." >&2
  usage
  exit 1
fi

# Validierung
if [[ ! -d "$QT_ROOT/bin" ]]; then
  echo "WARNUNG: Ungültiger QT_ROOT (bin fehlt): $QT_ROOT" >&2
  exit 1
fi

if [[ ! -x "$QT_ROOT/bin/qmake" && ! -x "$QT_ROOT/bin/qt-cmake" ]]; then
  echo "WARNUNG: Kein qmake/qt-cmake in $QT_ROOT/bin gefunden: $QT_ROOT" >&2
  exit 1
fi

# Qt nur für dieses Skript aktivieren
export PATH="$QT_ROOT/bin:$PATH"
export CMAKE_PREFIX_PATH="$QT_ROOT${CMAKE_PREFIX_PATH:+:$CMAKE_PREFIX_PATH}"

# Version
ARCH=x86_64
VERSION=$(git describe --always --tags)

# check and create a appimage dir
if [ ! -d ./appimage ]; then
mkdir ./appimage; 
else
    rm -rf ./appimage/AppDir
fi

cd appimage

MYDIR="`cd $0; pwd`"
echo "MYDIR is: " $MYDIR

mkdir build

cd build
qmake ../.. APPIMAGE_PATH="$MYDIR/AppDir"

make -j24
make install

cd ..
rm -rf build

cp ../src/AppRun ./AppDir
cp ../src/emu64.desktop ./AppDir          
cp ../grafik/emu64.png ./AppDir

# linuxdeploy 
if [ ! -f ./linuxdeploy-x86_64.AppImage ]; then
    wget "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
    chmod u+x ./linuxdeploy-x86_64.AppImage
fi

# linuxdeploy qt plugin
if [ ! -f ./linuxdeploy-plugin-qt-x86_64.AppImage ]; then
    wget "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
    chmod u+x ./linuxdeploy-plugin-qt-x86_64.AppImage
fi

# ohne Qt
# ARCH=$ARCH VERSION=$VERSION ./linuxdeploy-x86_64.AppImage --appdir ./AppDir --output appimage
# mit libc im appimage
# ARCH=$ARCH VERSION=$VERSION ./linuxdeploy-x86_64.AppImage --appdir ./AppDir -l /lib/x86_64-linux-gnu/libc.so.6 -l /lib/x86_64-linux-gnu/libm.so.6 -l /lib/x86_64-linux-gnu/libstdc++.so.6 -l /lib/x86_64-linux-gnu/libgcc_s.so.1 --plugin qt --output appimage

ARCH=$ARCH VERSION=$VERSION ./linuxdeploy-x86_64.AppImage --appdir ./AppDir --plugin qt --output appimage

rm -rf AppDir

tar -cvzf emu64_"$VERSION"_linux_appimage_"$ARCH".tar.gz Emu64-"$VERSION"-"$ARCH".AppImage
rm Emu64-"$VERSION"-"$ARCH".AppImage

# SHA512 Hashwert erzeugen
sha512sum -b emu64_"$VERSION"_linux_appimage_"$ARCH".tar.gz >> emu64_"$VERSION"_linux_appimage_"$ARCH".tar.gz.sha512
