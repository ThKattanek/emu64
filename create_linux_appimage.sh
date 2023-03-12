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
# // Letzte Änderung am 12.03.2023                //
# // www.emu64-projekt.de                         //
# //                                              //
# //////////////////////////////////////////////////

#!/bin/bash

declare project_name=emu64

declare public_release=true

# Version
version=$(git describe --always --tags)

# check and create a appimage dir
if [ ! -d ./appimage ]; then
mkdir ./appimage; 
else
    if [ $public_release = true ]; then
        rm -rf ./appimage/*
    fi
fi

cd appimage

mkdir build

cd build
qmake ../.. PREFIX="/home/thorsten/Programmierung/Aktuelle_Projekte/emu64/appimage/AppDir"      # Muss noch angepasst werden !!
make -j24
make install

cd ..
rm -rf build

cd AppDir
mkdir usr
mv bin ./usr
mv share ./usr

cd ..

cp ../src/emu64.desktop ./AppDir
cp ../src/AppRun ./AppDir               # Sollte eigtl direkt hier erstellt werden
cp ../grafik/emu64.png ./AppDir

# https://github.com/AppImage/AppImageKit/releases/tag/continuous/appimagetool-x86_64.AppImage


if [ ! -f ./linuxdeploy-x86_64.AppImage ]; then
    wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod u+x ./linuxdeploy-x86_64.AppImage
fi
ARCH=x86_64 ./linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage
#tar -cvzf my-game-linux-x86-64.tar.gz my-game-x86_64.AppImage
#echo "md5sum of my-game-linux-x86-64.tar.gz = " && md5sum my-game-linux-x86-64.tar.gz
