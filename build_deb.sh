#!/bin/bash
# Create a Debian Package

checkinstall -D \
    --install=no \
    --pkggroup="game, system" \
    --pkgsource="https://bitbucket.org/tkattanek/emu64/" \
    --pkglicense="LGPL" \
    --deldesc=no \
    --nodoc \
    --maintainer="Thorsten Kattanek\\<thorsten.kattanek@emu64-projekt.de\\>" \
    --pkgarch=$(dpkg \
    --print-architecture) \
    --pkgversion="5.0.16" \
    --pkgrelease="0" \
    --pkgname=emu64 \
    --requires="libc6, libx11-6, libsdl2-2.0-0, libsdl2-image-2.0-0, libpng16-16, libquazip1, libglu1-mesa, libavutil55, libavcodec57, libavformat57, libswscale4, libswresample2"
    

alien -r *.deb
alien -t *.deb
