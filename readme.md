![Logo](http://www.emu64-projekt.de/images/emu64_logo_120_120.png)

## Emu64 build under Linux

### Required Packages 
- qt-sdk
- git
- cmake
- libsdl2-dev
- libsdl2-image-dev
- libquazip-dev
- libpng-dev
- libavcodec-dev
- libavutil-dev
- libswresample-dev
- libavformat-dev
- libswscale-dev

### Build
```bash
git clone https://github.com/ThKattanek/emu64.git
cd emu64
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
