#!/bin/bash

# QtBin Pfad bitte hier eintragen
qt_path=/usr/local/qt/4.8.5/bin/

"$qt_path"/qmake -r -spec linux-g++-64 -o Makefile
make --makefile=Makefile -j8
