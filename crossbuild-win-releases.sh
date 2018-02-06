# //////////////////////////////////////////////////
# //                                              //
# // Emu64                                        //
# // von Thorsten Kattanek                        //
# //                                              //
# // #file: crossbuild-win-releases.sh            //
# //                                              //
# // Dieser Sourcecode ist Copyright geschützt!   //
# // Geistiges Eigentum von Th.Kattanek           //
# //                                              //
# // Letzte Änderung am 06.02.2018                //
# // www.emu64-projekt.de                         //
# //                                              //
# //////////////////////////////////////////////////

#!/bin/bash

# script by thorsten kattanek
# excample: crossbuild-win-releases.sh ~/mxe

declare project_name=emu64

# Version
version=$(git describe --always --tags)

# check of variable $1
if [ $1 ]; then
    declare mxe_path=$1
    # check of exist path from variable $1
    if [ ! -d $mxe_path ]; then
        echo "Not exist the MXE path: " $1
        exit
    fi
else
    echo "Please specify the MXE path (excample: crossbuild-win-releases.sh ~/mxe)"
    exit
fi    

# check and create a public_release dir
if [ ! -d ./public_release ]; then
mkdir ./public_release; 
else
    rm -rf ./public_release/*
fi

cd public_release

# check of cmake for static i686 and x86_64
declare i686_cmake=$mxe_path/usr/bin/i686-w64-mingw32.static-cmake
declare x86_64_cmake=$mxe_path/usr/bin/x86_64-w64-mingw32.static-cmake

if [ ! -e $i686_cmake ]; then
    echo "Cmake for static i686 is not exist!"
else
    declare i686_ok=true
fi

if [ ! -e $x86_64_cmake ]; then
    echo "Cmake for static x86_64 is not exist!"
else
    declare x86_64_ok=true
fi

#### 32Bit Static
if [ $i686_ok ]; then
    echo "Creating a i686-Static windows binary ..."
    
    # check and create a build dir
    declare build_i686_dir="./build-"$project_name"_"$version"_win_x32"
    if [ ! -d $build_i686_dir ]; then
    mkdir $build_i686_dir; 
    else
        rm -rf $build_i686_dir/*
    fi

    # check and delete if exist a install dir
    declare install_i686_dir="./"$project_name"_"$version"_win_x32"
    if [ -d $install_i686_dir ]; then
        rm -rf $install_i686_dir/*
    fi

    # execute cmake
    cd $build_i686_dir
    $i686_cmake ../.. -DCMAKE_INSTALL_PREFIX=../$install_i686_dir -DWIN32_STATIC_BUILD=TRUE
    make -j8 install
    cd ..
    
    rm -rf $build_i686_dir

    # compress as 7z
    echo "Release 32bit as 7z kompressed..."
    7z a -t7z -m0=LZMA -mmt=on -mx=9 -md=96m -mfb=256 $install_i686_dir".7z" $install_i686_dir

    rm -rf $install_i686_dir
fi

### 64Bit Static
if [ $x86_64_ok ]; then
    echo "Creating a x86_64-Static windows binary ..."

    # check and create a build dir
    declare build_x86_64_dir="./build-"$project_name"_"$version"_win_x64"
    if [ ! -d $build_x86_64_dir ]; then
    mkdir $build_x86_64_dir;
    else
        rm -rf $build_x86_64_dir/*
    fi

    # check and delete if exist a install dir
    declare install_x86_64_dir="./"$project_name"_"$version"_win_x64"
    if [ -d $install_x86_64_dir ]; then
        rm -rf $install_x86_64_dir/*
    fi

    # execute cmake
    cd $build_x86_64_dir
    $x86_64_cmake ../.. -DCMAKE_INSTALL_PREFIX=../$install_x86_64_dir -DWIN32_STATIC_BUILD=TRUE
    make -j8 install
    cd ..

    rm -rf $build_x86_64_dir

    # compress as 7z
    echo "Release 64bit as 7z kompressed..."
    7z a -t7z -m0=LZMA -mmt=on -mx=9 -md=96m -mfb=256 $install_x86_64_dir".7z" $install_x86_64_dir

    rm -rf $install_x86_64_dir
fi

cd ..

echo "--> Die fertigen Pakete befinden sich im Verzeichnis 'public_release' !"
echo ""
echo "E.N.D.E"
