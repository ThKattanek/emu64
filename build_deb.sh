#!/bin/bash
# Create a Debian Package

checkinstall -D \
    --install=no \
    --pkggroup="game, system" \
    --pkgsource="https://bitbucket.org/tkattanek/emu64/" \
    --pkglicense="LGPL" \
    --deldesc=no \
    --nodoc \
    --maintainer="Thorsten Kattanek\\<thorsten.Kattanek@emu64.de\\>" \
    --pkgarch=$(dpkg \
    --print-architecture) \
    --pkgversion="5.0.10" \
    --pkgrelease="0" \
    --pkgname=emu64 \
    --requires="libc6 \(\>= 2.4\),libgcc1 \(\>= 1:4.1.1\),libncurses5 \(\>= 5.5-5~\),libpcap0.8 \(\>= 0.9.8\),libstdc++6 \(\>= 4.1.1\),libtinfo5, \
		libqtcore4, libqtgui4, libsdl2-2.0-0 \(\>= 2-2.0-0\), libsdl2-image-2.0-0 \(\>= 2.0-0\), libquazip1"

alien -r *.deb