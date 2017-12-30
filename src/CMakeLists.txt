# Emu64 CMakeLists.txt
# by thorsten kattanek

cmake_minimum_required (VERSION 3.7)

project (emu64) 

# VersionString
add_definitions(-Dstr_emu64_version="\\"5.0.16\\"")

# ArchitectureString
add_definitions(-Dstr_system_arch="\\"LINUX\\"")

# add_subdirectory(src)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

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

# Qt4
find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )
include_directories( ${CMAKE_BINARY_DIR} )   

QT4_ADD_RESOURCES( RES_SOURCES ${PROJECT_SOURCE_DIR}/${RESOURCES} )
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )  

# includes cmake/FindSDL2.cmake cmake/FindSDL2_image.cmake cmake/FindSDL2_ttf.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

# SDL2 Image
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIR})

# X11
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# Quazip
find_package(QuaZip REQUIRED)
include_directories(${QUAZIP_INCLUDE_DIR})

# AVFormat
find_package(FFMPEG REQUIRED)
include_directories(${FFMPEG_INCLUDE_DIR})

# PNG
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)   
            
add_executable(emu64 ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS} )
target_link_libraries(emu64 ${QT_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${QUAZIP_LIBRARIES} ${FFMPEG_LIBRARIES} ${PNG_LIBRARIES})
              
if (UNIX)
    target_link_libraries(emu64 ${X11_LIBRARIES})
endif (UNIX)

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
install(FILES "floppy_sounds/FloppySound Hinweis.txt" DESTINATION share/emu64/floppy_sounds)
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
