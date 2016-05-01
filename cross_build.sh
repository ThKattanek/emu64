#!/bin/bash
# windows binary cross compiled with MXE and qmake (http://mxe.cc/)
# by thorsten kattanek 2016

if [ $# -ne 4 ] ; then       #Solange die Anzahl der Parameter ($#) größer 0
   echo ""
   echo "Fehler: Parameter fehlen ! (cross_build.sh MXE-Pfad Zielarchitektur Linktyp PRO-File)"
   echo ""
   echo "Beispiel: ./cross_build.sh /media/daten/mxe i686 static ~/emu64/emu64_projekt.pro"
   echo ""
   exit
fi

# Alle 3 Parameter zwischenspeichern
MXE_ROOT_PATH="${1}"
ARCH="${2}"
LINKTYP="${3}"
PRO_FILE="${4}"

###############################################################################

# Pfade zusammenstellen
BUILD_PATH="./emu64-win-${ARCH}-${LINKTYP}"
MXE_USR_PATH="$MXE_ROOT_PATH/usr"
MXE_BIN_PATH="$MXE_USR_PATH/bin" 

# Architektur Name für mingw zusammenstellen
ARCHNAME="${ARCH}-w64-mingw32.${LINKTYP}"

# Alle Parameter ausgeben
echo
echo "+++++++++++++++++++++++++++++++++++++++++++"
echo "MXE Root Pfad = ${MXE_ROOT_PATH}"
echo "Architektur = ${ARCH}"
echo "Linktyp = ${LINKTYP}"
echo "Name = ${ARCHNAME}"
echo "+++++++++++++++++++++++++++++++++++++++++++"
echo 

# Prüfen ob für die gewählte Architektur unter MXE eine copilierte Version vorhanden ist
if [ -d "${MXE_USR_PATH}/${ARCH}-w64-mingw32.${LINKTYP}" ] ; then
echo "Pakete für ${ARCH}-w64-mingw32.${LINKTYP} gefunden."
else
echo "Keine Pakete für ${ARCH}-w64-mingw32.${LINKTYP} gefunden !"
exit
fi

export PATH="$MXE_BIN_PATH":$PATH

# check of exist build dir
if [ -d "$BUILD_PATH" ] ; then
# build dir ist exist
 
echo "Buildverzeichnis schon vorhanden"
 
else
# build dir is not exist
# create build dir
mkdir "$BUILD_PATH"
fi

# change to build dir
cd "$BUILD_PATH"

# qmake run

${MXE_USR_PATH}/${ARCHNAME}/qt/bin/qmake ARCH=${ARCH} LINKTYP=${LINKTYP} ${PRO_FILE} -r

# build run
make -j8