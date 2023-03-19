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
# // Letzte Änderung am 13.03.2023                //
# // www.emu64-projekt.de                         //
# //                                              //
# //////////////////////////////////////////////////

#!/bin/bash

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
    wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod u+x ./linuxdeploy-x86_64.AppImage
fi

# linuxdeploy qt plugin
if [ ! -f ./linuxdeploy-plugin-qt-x86_64.AppImage ]; then
    wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
    chmod u+x ./linuxdeploy-plugin-qt-x86_64.AppImage
fi

# ohne Qt
# ARCH=$ARCH VERSION=$VERSION ./linuxdeploy-x86_64.AppImage --appdir ./AppDir --output appimage
# mit libc im appimage
# ARCH=$ARCH VERSION=$VERSION ./linuxdeploy-x86_64.AppImage --appdir ./AppDir -l /lib/x86_64-linux-gnu/libc.so.6 -l /lib/x86_64-linux-gnu/libm.so.6 -l /lib/x86_64-linux-gnu/libstdc++.so.6 -l /lib/x86_64-linux-gnu/libgcc_s.so.1 --plugin qt --output appimage

ARCH=$ARCH VERSION=$VERSION ./linuxdeploy-x86_64.AppImage --appdir ./AppDir --plugin qt --output appimage

rm -rf AppDir

tar -cvzf emu64_"$VERSION"_linux_"$ARCH".tar.gz Emu64-"$VERSION"-"$ARCH".AppImage
rm Emu64-"$VERSION"-"$ARCH".AppImage

# SHA512 Hashwert erzeugen
sha512sum -b emu64_"$VERSION"_linux_"$ARCH".tar.gz >> emu64_"$VERSION"_linux_"$ARCH".tar.gz.sha512
