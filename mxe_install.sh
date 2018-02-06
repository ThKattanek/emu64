#!/bin/bash
# MXE - build-2017-02-11
# by th.kattanek in berlin, 2018-02-06 
# Installed all next libs as static in i686 and x86_64
# Qt            4.8.7
# SDL2          2.0.5
# SDL2_image    2.0.0
# FFMPEG        3.2.1
# SFML          2.3.2
# Quazip        0.7.3
#
# Installed at home (~)

patch=true
build_qt=true
build_sdl2=true
build_ffmpeg=true
build_quazip=true
build_sfml=false

cd ~
git clone https://github.com/mxe/mxe.git
cd mxe
git checkout tags/build-2017-02-11

MYDIR="`cd $0; pwd`"
echo $MYDIR

export PATH="$PATH:$MYDIR/usr/bin"

if [ $patch = true ];then
(    
    # Patch -- Checksumme in /src/protobuf.mk patchen
    echo "--- protobuf_old.mk	2018-02-06 11:19:04.447333559 +0100" > protobuf.patch
    echo "+++ protobuf.mk	2018-02-06 11:19:39.512823260 +0100" >> protobuf.patch
    echo "@@ -4,7 +4,7 @@ PKG             := protobuf" >> protobuf.patch
    echo " \$(PKG)_WEBSITE  := https://github.com/google/protobuf" >> protobuf.patch
    echo " \$(PKG)_IGNORE   :=" >> protobuf.patch
    echo " \$(PKG)_VERSION  := 3.1.0" >> protobuf.patch
    echo "-\$(PKG)_CHECKSUM := 0a0ae63cbffc274efb573bdde9a253e3f32e458c41261df51c5dbc5ad541e8f7" >> protobuf.patch
    echo "+\$(PKG)_CHECKSUM := fb2a314f4be897491bb2446697be693d489af645cb0e165a85e7e64e07eb134d" >> protobuf.patch
    echo " \$(PKG)_SUBDIR   := \$(PKG)-\$(\$(PKG)_VERSION)" >> protobuf.patch
    echo " \$(PKG)_FILE     := \$(PKG)-\$(\$(PKG)_VERSION).tar.gz" >> protobuf.patch
    echo " \$(PKG)_URL      := https://github.com/google/\$(PKG)/archive/v\$(\$(PKG)_VERSION).tar.gz" >> protobuf.patch
    patch src/protobuf.mk protobuf.patch
)
fi

if [ $build_qt = true ];then
(
    make MXE_TARGETS='i686-w64-mingw32.static x86_64-w64-mingw32.static' qt -j 4 JOBS=2
)
fi

if [ $build_sdl2 = true ];then
(
    make MXE_TARGETS='i686-w64-mingw32.static x86_64-w64-mingw32.static' sdl2 sdl2_gfx sdl2_image sdl2_mixer sdl2_net sdl2_ttf -j 4 JOBS=2
)
fi

if [ $build_ffmpeg = true ];then
(
    make MXE_TARGETS='i686-w64-mingw32.static x86_64-w64-mingw32.static' ffmpeg -j 4 JOBS=2
)
fi

if [ $build_sfml = true ];then
(
    make MXE_TARGETS='i686-w64-mingw32.static x86_64-w64-mingw32.static' sfml -j 4 JOBS=2
)
fi

if [ $build_quazip = true ];then
(
    echo "Quazip erstellen ..."
    
    if [ -d quazip_temp ];then
    (
        rm -rf quazip_temp
    )
    fi
    
    mkdir quazip_temp
    cd quazip_temp
    
    wget 'http://sourceforge.net/projects/quazip/files/quazip/0.7.3/quazip-0.7.3.tar.gz'
    
    echo "Architecture: i686"
    if [ ! -f quazip-0.7.3.tar.gz ];then
    (
        echo "Fehler: quazip-0.7.3.tar.gz konnte nicht runtergeladen werden !"
        exit
    )
    fi
    
    tar -xzf quazip-0.7.3.tar.gz -C ./
    
    if [ ! -d quazip-0.7.3 ];then
    (
        echo "Fehler: Verzeichnis quazip-0.7.3 konnte nicht entpackt werden !"
        exit
    )
    fi
    
    cd quazip-0.7.3/quazip
    
    ~/mxe/usr/i686-w64-mingw32.static/qt/bin/qmake PREFIX=~/mxe/usr/i686-w64-mingw32.static
    make -j8
    make install

    cd ../..
    rm -rf quazip-0.7.3
    
    echo "Architecture: x86_64"    
    if [ ! -f quazip-0.7.3.tar.gz ];then
    (
        echo "Fehler: quazip-0.7.3.tar.gz konnte nicht runtergeladen werden !"
        exit
    )
    fi
    
    tar -xzf quazip-0.7.3.tar.gz -C ./
    
    if [ ! -d quazip-0.7.3 ];then
    (
        echo "Fehler: Verzeichnis quazip-0.7.3 konnte nicht entpackt werden !"
        exit
    )
    fi
    
    cd quazip-0.7.3/quazip
    
    ~/mxe/usr/x86_64-w64-mingw32.static/qt/bin/qmake PREFIX=~/mxe/usr/x86_64-w64-mingw32.static
    make -j8
    make install

    cd ../../..
    
    rm -rf quazip_temp
)
fi

make clean-junk
