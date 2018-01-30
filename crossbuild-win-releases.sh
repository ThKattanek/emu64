#!/bin/bash

# script by thorsten kattanek
# excample: build-windows-release.sh ~/MXE

declare project_name=sfml-demo

# check of variable $1
if [ $1 ]; then
    declare dir=$1
    # check of exist path from variable $1
    if [ ! -d $dir ]; then
        echo "Not exist the path: " $1
        exit
    fi
else
    echo "Please specify the MXE path (excample: build-windows-release.sh ~/MXE)"
    exit
fi    

# check of cmake for static i686 and x86_64
declare i686_cmake=$1/usr/bin/i686-w64-mingw32.static-cmake
declare x86_64_cmake=$1/usr/bin/x86_64-w64-mingw32.static-cmake

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
    declare build_i686_dir=./build_win_32bit
    if [ ! -d $build_i686_dir ]; then
    mkdir $build_i686_dir; 
    else
        rm -rf $build_i686_dir/*
    fi
    
    # check and delete if exist a install dir
    declare install_i686_dir=./install_win_32bit
    if [ -d $install_i686_dir ]; then
        rm -rf $install_i686_dir/*
    fi
    
    # execute cmake
    cd $build_i686_dir
    $i686_cmake .. -DSFML_STATIC_LIBRARIES=TRUE -DCMAKE_INSTALL_PREFIX=../$install_i686_dir
    make install
    cd ..
    
    #rm -rf $build_i686_dir
    
    # compress as 7z
    echo "Release 32bit as 7z kompressed..."
    7z a -t7z -m0=LZMA -mmt=on -mx=9 -md=96m -mfb=256 $install_i686_dir/$project_name-win-32bit.7z $install_i686_dir/*
fi

### 64Bit Static
if [ $x86_64_ok ]; then
    echo "Creating a x86_64-Static windows binary ..."
    
    # check and create a build dir
    declare build_x86_64_dir=./build_win_64bit
    if [ ! -d $build_x86_64_dir ]; then
        mkdir $build_x86_64_dir; 
    else
        rm -rf $build_x86_64_dir/*
    fi
    
    # check and delete if exist a install dir
    declare install_x86_64_dir=./install_win_64bit
    if [ -d $install_x86_64_dir ]; then
        rm -rf $install_x86_64_dir/*
    fi
    
    # execute cmake
    cd $build_x86_64_dir
    $x86_64_cmake .. -DSFML_STATIC_LIBRARIES=TRUE -DCMAKE_INSTALL_PREFIX=../$install_x86_64_dir
    make install
    cd ..
    
    #rm -rf $build_x86_64_dir
  
    # compress as 7z
    echo "Release 64bit as 7z kompressed..."
    7z a -t7z -m0=LZMA -mmt=on -mx=9 -md=96m -mfb=256 $install_x86_64_dir/$project_name-win-64bit.7z $install_x86_64_dir/*
fi
