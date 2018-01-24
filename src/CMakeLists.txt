# Emu64 CMakeLists.txt
# by thorsten kattanek

cmake_minimum_required (VERSION 3.7)

project (emu64) 

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

add_definitions("-DGIT_COMMIT_HASH=${GIT_COMMIT_HASH}")
add_definitions("-DGIT_BRANCH=${GIT_BRANCH}")
add_definitions("-DVERSION_STRING=${VERSION_STRING}")

# ArchitectureString

if ( "${CMAKE_SIZEOF_VOID_P}" EQUAL 4)
        add_definitions(-Dstr_system_arch="32Bit")
elseif ( "${CMAKE_SIZEOF_VOID_P}" EQUAL 8)
        add_definitions(-Dstr_system_arch="64Bit")
endif()

configure_file(
  ${CMAKE_SOURCE_DIR}/src/version.h.in
  ${CMAKE_BINARY_DIR}/src/version.h
)

if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(SOURCES
                am29f040_class.cpp
                button_mod.cpp
                c64_class.cpp
                c64_keyboard_window.cpp
                c64_speed_window.cpp
                command_line_class.cpp
                crt_class.cpp
                crt_new_easyflash_window.cpp
                crt_window.cpp
                custom_splashscreen.cpp
                d64_class.cpp
                debugger_iec_window.cpp
                debugger_vic_window.cpp
                debugger_window.cpp
                floppy1541_class.cpp
                floppy_new_d64_window.cpp
                floppy_window.cpp
                georam_class.cpp
                info_window.cpp
                label_widget_mod.cpp
                main.cpp
                main_window.cpp
                memory_window.cpp
                mmu_class.cpp
                mos6502_class.cpp
                mos6510_class.cpp
                mos6522_class.cpp
                mos6526_class.cpp
                mos6526_port.cpp
                mos6569_class.cpp
                mos6581_8085_class.cpp
                new_romset_window.cpp
                reu_class.cpp
                savepng.c
                setup_window.cpp
                show_c64_key_mapping_window.cpp
                siddump.cpp
                single_application.cpp
                tape1530_class.cpp
                tape_new_window.cpp
                tape_window.cpp
                tree_widget_mod.cpp
                tv_setup_window.cpp
                vcd_class.cpp
                video_capture_class.cpp
                video_capture_window.cpp
                videopal_class.cpp
                widget_bit_leiste.cpp
                widget_file_browse.cpp
                widget_floppy_status.cpp
                widget_memory_zeile.cpp
                widget_sr_edit.cpp
                widget_tape_counter.cpp
)

set(MOC_HEADERS
                button_mod.h
                c64_keyboard_window.h
                c64_speed_window.h
                command_line_class.h
                crt_new_easyflash_window.h
                crt_window.h
                debugger_iec_window.h
                debugger_vic_window.h
                debugger_window.h
                floppy_new_d64_window.h
                floppy_window.h
                info_window.h
                label_widget_mod.h
                main_window.h
                memory_window.h
                new_romset_window.h
                setup_window.h
                show_c64_key_mapping_window.h
                single_application.h
                tape_new_window.h
                tape_window.h
                tree_widget_mod.h
                tv_setup_window.h
                video_capture_window.h
                widget_bit_leiste.h
                widget_file_browse.h
                widget_floppy_status.h
                widget_memory_zeile.h
                widget_sr_edit.h
                widget_tape_counter.h
)

set(UIS
                c64_keyboard_window.ui
                c64_speed_window.ui
                crt_new_easyflash_window.ui
                crt_window.ui
                debugger_iec_window.ui
                debugger_vic_window.ui
                debugger_window.ui
                floppy_new_d64_window.ui
                floppy_window.ui
                info_window.ui
                main_window.ui
                memory_window.ui
                new_romset_window.ui
                setup_window.ui
                show_c64_key_mapping_window.ui
                tape_new_window.ui
                tape_window.ui
                tv_setup_window.ui
                video_capture_window.ui
                widget_bit_leiste.ui
                widget_file_browse.ui
                widget_floppy_status.ui
                widget_memory_zeile.ui
                widget_sr_edit.ui
                widget_tape_counter.ui
)

set(RESOURCES
                emu64.qrc
)

SET(TRANS
                emu64_de.ts
                emu64_en.ts
)

if(WIN32)
    set(WIN_RC
                emu64.rc
        )
endif()

# Qt4
find_package( Qt4 REQUIRED )
if(QT4_FOUND)
    message(STATUS "Qt4 wurde gefunden.")
    include( ${QT_USE_FILE} )
    add_definitions( ${QT_DEFINITIONS} )
    include_directories( ${CMAKE_BINARY_DIR})

    QT4_ADD_TRANSLATION(QM ${TRANS})
    QT4_ADD_RESOURCES( RES_SOURCES ${PROJECT_SOURCE_DIR}/${RESOURCES} )
    QT4_WRAP_UI( UI_HEADERS ${UIS} )
    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(emu64 ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS} ${QM} ${WIN_RC})

# SDL2
find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
    message(STATUS "SDL2 wurde gefunden.")
    include_directories(${SDL2_INCLUDE_DIR})
    target_link_libraries(emu64 ${SDL2_LIBRARY})
    #add_definitions(${SDL2_CFLAGS} ${SDL2_CFLAGS_OTHER})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SDL2_CFLAGS} ${SDL2_CFLAGS_OTHER}")
else()
    message(FATAL_ERROR "SDL2 wurde nicht gefunden.")
endif()

# SDL2 Image
find_package(SDL2_image REQUIRED)
if(SDL2_IMAGE_FOUND)
    message(STATUS "SDL2_image wurde gefunden.")
    include_directories(${SDL2_IMAGE_INCLUDE_DIR})
    target_link_libraries(emu64 ${SDL2_IMAGE_LIBRARY})
    #add_definitions(${SDL2_IMAGE_CFLAGS} ${SDL2_IMAGE_CFLAGS_OTHER})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SDL2_IMAGE_CFLAGS} ${SDL2_IMAGE_CFLAGS_OTHER}")
else()
    message(FATAL_ERROR "SDL2_IMAGE wurde nicht gefunden.")
endif()

if (UNIX)
    # X11
    find_package(X11 REQUIRED)
    if(X11_FOUND)
        message(STATUS "X11 wurde gefunden.")
        include_directories(${X11_INCLUDE_DIR})
        target_link_libraries(emu64 ${X11_LIBRARIES})
    endif()
endif (UNIX)

# OpenGL
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
    message(STATUS "OpenGL wurde gefunden.")
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(emu64 ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPENGL_gl_CFLAGS} ${OPENGL_gl_CFLAGS_OTHER}")
else()
    message(FATAL_ERROR "OPENGL wurde nicht gefunden.")
endif()

# Quazip
find_package(QuaZip REQUIRED)
if(QUAZIP_FOUND)
    message(STATUS "Quazip wurde gefunden.")
    include_directories(${QUAZIP_INCLUDE_DIR})
    target_link_libraries(emu64 ${QUAZIP_LIBRARIES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QUAZIP_CFLAGS} ${QUAZIP_CFLAGS_OTHER}")
else()
    message(FATAL_ERROR "QUAZIP wurde nicht gefunden.")
endif()

# FFMPEG
find_package(FFMPEG REQUIRED)
if(FFMPEG_FOUND)
    message(STATUS "FFMPEG wurde gefunden.")
    include_directories(${FFMPEG_INCLUDE_DIR})
    target_link_libraries(emu64 ${FFMPEG_LIBRARIES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FFMPEG_CFLAGS} ${FFMPEG_CFLAGS_OTHER}")
else()
    message(FATAL_ERROR "FFMPEG wurde nicht gefunden.")
endif()

# PNG
find_package(PNG REQUIRED)
if(PNG_FOUND)
    message(STATUS "PNG wurde gefunden.")
    include_directories(${PNG_INCLUDE_DIR})
    target_link_libraries(emu64 ${PNG_LIBRARIES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PNG_CFLAGS} ${PNG_CFLAGS_OTHER}")
else()
    message(FATAL_ERROR "PNG wurde nicht gefunden.")
endif()

message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})

target_link_libraries(emu64 ${QT_LIBRARIES})

# Set DataPath
add_definitions("-DDATA_PATH=\"${CMAKE_INSTALL_PREFIX}\"")

# Install Binary
install(TARGETS emu64 DESTINATION bin)

# Install C64 Roms
install(FILES "roms/kernal.rom" DESTINATION share/emu64/roms)
install(FILES "roms/char.rom" DESTINATION share/emu64/roms)
install(FILES "roms/basic.rom" DESTINATION share/emu64/roms)
install(FILES "roms/1541.rom" DESTINATION share/emu64/roms)

# Install Floppysounds
install(FILES "floppy_sounds/hinweis.txt" DESTINATION share/emu64/floppy_sounds)
install(FILES "floppy_sounds/stepper_inc.raw" DESTINATION share/emu64/floppy_sounds)
install(FILES "floppy_sounds/stepper_dec.raw" DESTINATION share/emu64/floppy_sounds)
install(FILES "floppy_sounds/motor_on.raw" DESTINATION share/emu64/floppy_sounds)
install(FILES "floppy_sounds/motor_off.raw" DESTINATION share/emu64/floppy_sounds)
install(FILES "floppy_sounds/motor.raw" DESTINATION share/emu64/floppy_sounds)
install(FILES "floppy_sounds/anschlag.raw" DESTINATION share/emu64/floppy_sounds)

# Install GFX
install(FILES "grafik/kreis0.png" DESTINATION share/emu64/gfx)
install(FILES "grafik/kreis1.png" DESTINATION share/emu64/gfx)
install(FILES "grafik/pfeil0.png" DESTINATION share/emu64/gfx)
install(FILES "grafik/pfeil1.png" DESTINATION share/emu64/gfx)
install(FILES "grafik/sdl_icon.png" DESTINATION share/emu64/gfx)

# Install Languages
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/emu64_de.qm" DESTINATION share/emu64/languages)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/emu64_en.qm" DESTINATION share/emu64/languages)
install(FILES "grafik/flaggen/emu64_de.png" DESTINATION share/emu64/languages)
install(FILES "grafik/flaggen/emu64_en.png" DESTINATION share/emu64/languages)

# Install Startmenü Eintrag
install(FILES "emu64.desktop" DESTINATION share/applications)
install(FILES "grafik/emu64.png" DESTINATION share/icons/hicolor/64x64/apps)
