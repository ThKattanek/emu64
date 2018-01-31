# //////////////////////////////////////////////////
# //                                              //
# // Emu64                                        //
# // von Thorsten Kattanek                        //
# //                                              //
# // #file: CMakeLists.txt                        //
# //                                              //
# // Dieser Sourcecode ist Copyright geschützt!   //
# // Geistiges Eigentum von Th.Kattanek           //
# //                                              //
# // Letzte Änderung am 30.01.2018                //
# // www.emu64.de                                 //
# //                                              //
# //////////////////////////////////////////////////

###################################################
# Options
# -CMAKE_INSTALL_PREFIX=[INSTALL_PATH] --- Setzt den Installationspfad
# -DWIN32_STATIC_BUILD (TRUE or FLASE) --- Wenn TRUE wird ein Windows Static Binarie erstellt
# -DCMAKE_BUILD_TYPE (Release or Debug) --- Build Typ welches erzeugt werden soll (Default is Release)

cmake_minimum_required(VERSION 2.8)

##################### PROJEKT #####################

project("emu64")
message(STATUS "Project Name: " ${PROJECT_NAME})

################## CONFIGURATION ##################

# Standard Build Typ = Release
if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
        message(STATUS "CMAKE_BUILD_TYPE NOT FOUND -- Default setting to Release")
endif()

# Standard WIN32_STATIC_BUILD = FALSE
if(NOT WIN32_STATIC_BUILD)
        set(WIN32_STATIC_BUILD FALSE CACHE STRING "Choose a static or shared win32 build (TRUE or FALSE)" FORCE)
        message(STATUS "WIN32_STATIC_BUILD NOT FOUND -- Default setting to FALSE")
endif()

if(WIN32)
    if(WIN32_STATIC_BUILD)
        message(STATUS "BUILD A W32 STATIC BINARY")
        add_definitions("-DQUAZIP_STATIC")
    endif()
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

############### COMPILER OPTIONEN #################

if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

############ VERSIONSNUMMER via GIT ###############

# Holen des aktuellen Arbeitsbranches
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Holen des Hashecodes vom letzten Commits
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Version aus dem Tag ermitteln
execute_process(
  COMMAND git describe --always --tags
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE VERSION_STRING
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions("-DVERSION_STRING=\"${VERSION_STRING}\"")

message(STATUS "Aktueller Git Hashcode : ${GIT_COMMIT_HASH}")
message(STATUS "Aktueller Arbeitsbranch: ${GIT_BRANCH}")
message(STATUS "Aktuelle Emu64 Version : ${VERSION_STRING}")

############## ARCHITEKTUR ERMITTELN ##############

if ( "${CMAKE_SIZEOF_VOID_P}" EQUAL 4)
        set(ARCHITECTURE "32Bit")
elseif ( "${CMAKE_SIZEOF_VOID_P}" EQUAL 8)
        set(ARCHITECTURE "64Bit")
endif()

add_definitions("-DARCHITECTURE_STRING=\"${ARCHITECTURE}\"")

message(STATUS "Architektur: ${ARCHITECTURE}")

##################### SOURCES #####################

set(SOURCES
    src/am29f040_class.cpp
    src/button_mod.cpp
    src/c64_class.cpp
    src/c64_keyboard_window.cpp
    src/c64_speed_window.cpp
    src/command_line_class.cpp
    src/crt_class.cpp
    src/crt_new_easyflash_window.cpp
    src/crt_window.cpp
    src/custom_splashscreen.cpp
    src/d64_class.cpp
    src/debugger_iec_window.cpp
    src/debugger_vic_window.cpp
    src/debugger_window.cpp
    src/floppy1541_class.cpp
    src/floppy_new_d64_window.cpp
    src/floppy_window.cpp
    src/georam_class.cpp
    src/info_window.cpp
    src/label_widget_mod.cpp
    src/main.cpp
    src/main_window.cpp
    src/memory_window.cpp
    src/mmu_class.cpp
    src/mos6502_class.cpp
    src/mos6510_class.cpp
    src/mos6522_class.cpp
    src/mos6526_class.cpp
    src/mos6526_port.cpp
    src/mos6569_class.cpp
    src/mos6581_8085_class.cpp
    src/new_romset_window.cpp
    src/reu_class.cpp
    src/savepng.c
    src/setup_window.cpp
    src/show_c64_key_mapping_window.cpp
    src/siddump.cpp
    src/single_application.cpp
    src/tape1530_class.cpp
    src/tape_new_window.cpp
    src/tape_window.cpp
    src/tree_widget_mod.cpp
    src/tv_setup_window.cpp
    src/vcd_class.cpp
    src/video_capture_class.cpp
    src/video_capture_window.cpp
    src/videopal_class.cpp
    src/widget_bit_leiste.cpp
    src/widget_file_browse.cpp
    src/widget_floppy_status.cpp
    src/widget_memory_zeile.cpp
    src/widget_sr_edit.cpp
    src/widget_tape_counter.cpp
)

################## MOC HEADERS ####################

set(MOC_HEADERS
    src/button_mod.h
    src/c64_keyboard_window.h
    src/c64_speed_window.h
    src/command_line_class.h
    src/crt_new_easyflash_window.h
    src/crt_window.h
    src/debugger_iec_window.h
    src/debugger_vic_window.h
    src/debugger_window.h
    src/floppy_new_d64_window.h
    src/floppy_window.h
    src/info_window.h
    src/label_widget_mod.h
    src/main_window.h
    src/memory_window.h
    src/new_romset_window.h
    src/setup_window.h
    src/show_c64_key_mapping_window.h
    src/single_application.h
    src/tape_new_window.h
    src/tape_window.h
    src/tree_widget_mod.h
    src/tv_setup_window.h
    src/video_capture_window.h
    src/widget_bit_leiste.h
    src/widget_file_browse.h
    src/widget_floppy_status.h
    src/widget_memory_zeile.h
    src/widget_sr_edit.h
    src/widget_tape_counter.h
)

###################### UIS ########################

set(UIS
    src/c64_keyboard_window.ui
    src/c64_speed_window.ui
    src/crt_new_easyflash_window.ui
    src/crt_window.ui
    src/debugger_iec_window.ui
    src/debugger_vic_window.ui
    src/debugger_window.ui
    src/floppy_new_d64_window.ui
    src/floppy_window.ui
    src/info_window.ui
    src/main_window.ui
    src/memory_window.ui
    src/new_romset_window.ui
    src/setup_window.ui
    src/show_c64_key_mapping_window.ui
    src/tape_new_window.ui
    src/tape_window.ui
    src/tv_setup_window.ui
    src/video_capture_window.ui
    src/widget_bit_leiste.ui
    src/widget_file_browse.ui
    src/widget_floppy_status.ui
    src/widget_memory_zeile.ui
    src/widget_sr_edit.ui
    src/widget_tape_counter.ui
)

################### RESOURCEN #####################

set(QT_RC ${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}.qrc)

if(WIN32)
    set(WIN_RC ${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}.rc)
endif()

################# TRANSLATIONS ####################

SET(TRANS
    src/emu64_de.ts
    src/emu64_en.ts
)

##################### LIBRARY #####################

# X11
if (UNIX)
    find_package(X11 REQUIRED)
    if(X11_FOUND)
        include_directories(${X11_INCLUDE_DIR})
        set(LIBRARIES ${LIBRARIES} ${X11_LIBRARIES})
    else()
        message(FATAL_ERROR "X11 wurde nicht gefunden!")
    endif()
endif()

# Qt4
find_package(Qt4 REQUIRED )
if(QT4_FOUND)
    include(${QT_USE_FILE})
    add_definitions(${QT_DEFINITIONS})
    set(LIBRARIES ${LIBRARIES} ${QT_LIBRARIES})
    include_directories("${PROJECT_SOURCE_DIR}/src")
else()
    message(FATAL_ERROR "Qt4 wurde nicht gefunden!")
endif()

# Quazip
find_package(QuaZip REQUIRED)
if(QUAZIP_FOUND)
    include_directories(${QUAZIP_INCLUDE_DIR})
    set(LIBRARIES ${LIBRARIES} ${QUAZIP_LIBRARIES})
else()
    message(FATAL_ERROR "QUAZIP wurde nicht gefunden.")
endif()

# OpenGL
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    set(LIBRARIES ${LIBRARIES} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
else()
    message(FATAL_ERROR "OPENGL wurde nicht gefunden.")
endif()

# SDL2
find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIR})
    set(LIBRARIES ${LIBRARIES} ${SDL2_LIBRARY})
else()
    message(FATAL_ERROR "SDL2 wurde nicht gefunden!")
endif()

# SDL2 Image
find_package(SDL2_image REQUIRED)
if(SDL2_IMAGE_FOUND)
    include_directories(${SDL2_IMAGE_INCLUDE_DIR})
    set(LIBRARIES ${LIBRARIES} ${SDL2_IMAGE_LIBRARY})
else()
    message(FATAL_ERROR "SDL2_IMAGE wurde nicht gefunden.")
endif()

# PNG
find_package(PNG REQUIRED)
if(PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIR})
    set(LIBRARIES ${LIBRARIES} ${PNG_LIBRARIES})
else()
    message(FATAL_ERROR "PNG wurde nicht gefunden.")
endif()

# FFMPEG
find_package(FFMPEG REQUIRED)
if(FFMPEG_FOUND)
    include_directories(${FFMPEG_INCLUDE_DIR})
    set(LIBRARIES ${LIBRARIES} ${FFMPEG_LIBRARIES})
else()
    message(FATAL_ERROR "FFMPEG wurde nicht gefunden.")
endif()

# Für Windows
if(WIN32)
    #set(LIBRARIES ${LIBRARIES} -mwindows)  # Damit kein Konsolenfenster geöffnet wird
    if(WIN32_STATIC_BUILD)
        set(LIBRARIES ${LIBRARIES}
            avcodec avformat swresample swscale avutil quazip
            secur32 version  bluray bz2 freetype harfbuzz
            glib-2.0 iconv intl jpeg mp3lame ogg
            opencore-amrnb opencore-amrwb opus pcre samplerate
            speex theoradec theoraenc tiff vo-amrwbenc vorbis vorbisenc
            webp x264 xvidcore z vpx xml2)
    endif()
endif()

###################################################

QT4_ADD_TRANSLATION(QM ${TRANS})
QT4_ADD_RESOURCES(RES_SOURCES ${QT_RC})
QT4_WRAP_UI(UI_HEADERS ${UIS})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

################### EXECUTABLE ####################

if(NOT WIN32)
    add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS} ${QM})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS} ${QM} ${WIN_RC})
endif()

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

################### INSTALL #######################

# Set DataPath
add_definitions("-DDATA_PATH=\"${CMAKE_INSTALL_PREFIX}\"")

############ Install Binary und Daten #############
if(WIN32)

    ################## Windows #################
    # Install Binary
    install(TARGETS ${PROJECT_NAME} DESTINATION ./)

    # Install all Datas
    # Install C64 Roms
    install(DIRECTORY "roms" DESTINATION ./)

    # Install Floppysounds
    install(DIRECTORY "floppy_sounds" DESTINATION ./)

    # Install GFX
    install(FILES "grafik/kreis0.png" DESTINATION ./gfx)
    install(FILES "grafik/kreis1.png" DESTINATION ./gfx)
    install(FILES "grafik/pfeil0.png" DESTINATION ./gfx)
    install(FILES "grafik/pfeil1.png" DESTINATION ./gfx)
    install(FILES "grafik/sdl_icon.png" DESTINATION ./gfx)

    # Install Languages
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/emu64_de.qm" DESTINATION ./languages)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/emu64_en.qm" DESTINATION ./languages)
    install(FILES "grafik/flaggen/emu64_de.png" DESTINATION ./languages)
    install(FILES "grafik/flaggen/emu64_en.png" DESTINATION ./languages)

    # Lincense txt
    install(FILES "LICENSE" DESTINATION ./)

else()
    ################## Linux #################
    # Install Binary
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)

    # Install all Datas
    # Install C64 Roms
    install(DIRECTORY "roms" DESTINATION share/${PROJECT_NAME})

    # Install Floppysounds
    install(DIRECTORY "floppy_sounds" DESTINATION share/${PROJECT_NAME})

    # Install GFX
    install(FILES "grafik/kreis0.png" DESTINATION share/${PROJECT_NAME}/gfx)
    install(FILES "grafik/kreis1.png" DESTINATION share/${PROJECT_NAME}/gfx)
    install(FILES "grafik/pfeil0.png" DESTINATION share/${PROJECT_NAME}/gfx)
    install(FILES "grafik/pfeil1.png" DESTINATION share/${PROJECT_NAME}/gfx)
    install(FILES "grafik/sdl_icon.png" DESTINATION share/${PROJECT_NAME}/gfx)

    # Install Languages
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/emu64_de.qm" DESTINATION share/${PROJECT_NAME}/languages)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/emu64_en.qm" DESTINATION share/${PROJECT_NAME}/languages)
    install(FILES "grafik/flaggen/emu64_de.png" DESTINATION share/${PROJECT_NAME}/languages)
    install(FILES "grafik/flaggen/emu64_en.png" DESTINATION share/${PROJECT_NAME}/languages)

    # Install Startmenü Eintrag
    install(FILES "src/emu64.desktop" DESTINATION share/applications)
    install(FILES "grafik/emu64.png" DESTINATION share/icons/hicolor/64x64/apps)

    # Lincense txt
    install(FILES "LICENSE" DESTINATION share/${PROJECT_NAME})

endif()
