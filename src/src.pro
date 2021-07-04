# //////////////////////////////////////////////////
# //                                              //
# // Emu64                                        //
# // von Thorsten Kattanek                        //
# //                                              //
# // #file: scr.pro                               //
# //                                              //
# // Dieser Sourcecode ist Copyright geschützt!   //
# // Geistiges Eigentum von Th.Kattanek           //
# //                                              //
# // Letzte Änderung am 29.06.2021                //
# // www.emu64.de                                 //
# //                                              //
# //////////////////////////////////////////////////

!win32:isEmpty(PREFIX):PREFIX=/usr/local

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4){

QT += widgets

greaterThan(QT_MAJOR_VERSION, 5){
   QT += core5compat
}

TARGET = emu64
TEMPLATE = app

CONFIG += lrelease c++11

win32 {
    CONFIG += console
}

EMU64_VERSION = 5.0.18

# Versionsnummer ermitteln aus Git Tag Nummer
GIT_VERSION = $$system(git --git-dir \"../.git\" describe --always --tags)
isEmpty(GIT_VERSION) {
  GIT_VERSION = $$EMU64_VERSION
}
DEFINES += VERSION_STRING=\\\"$$GIT_VERSION\\\"

message("Emu64 Version: " $$GIT_VERSION)

contains(QT_ARCH, x86_64){
    EMU64_ARCH = 64Bit
} else:contains(QT_ARCH, i[3456]86) {
    EMU64_ARCH = 32Bit
} else {
    EMU64_ARCH = Unknown
}

DEFINES += ARCHITECTURE_STRING=\\\"$$EMU64_ARCH\\\"

win32 {
    message("Compiling for Windows $$EMU64_ARCH system")
} else:linux {
    message("Compiling for Linux $$EMU64_ARCH system")
} else:freebsd {
    message("Compiling for FreeBSD $$EMU64_ARCH system")
} else:unix {
    message("Compiling for Unix $$EMU64_ARCH system")
} else {
    message("Compiling for Unknown $$EMU64_ARCH system")
}

# Abhängigkeiten

CONFIG += link_pkgconfig
PKGCONFIG += sdl2 SDL2_image libpng glu libavutil libavformat libavcodec libswresample libswscale gl

# Für Win32 / Libs ohne PKCONFIG angeben
#LIBS += -lsdl2 -lsdl2main -lsdl2_image -lpng -lglu32 -lopengl32 -lavutil -lavformat -lavcodec -lswresample -lswscale

message("Zip: $$ZIP")

equals(QT_MAJOR_VERSION, 5) {

    DEFINES += ZIP_SUPPORT

    win32 {
        PKGCONFIG += quazip
    } else {
        LIBS += -lquazip5
    }

    message("QT5 is active")
}

equals(QT_MAJOR_VERSION, 6) {

    #DEFINES += ZIP_SUPPORT

    #win32 {
    #    PKGCONFIG += quazip
    #} else {
    #    LIBS += -lquazip6
    #}

    message("QT6 is active")
}


# Quelltexte

SOURCES += \
    am29f040_class.cpp \
    button_mod.cpp \
    c64_class.cpp \
    c64_keyboard_window.cpp \
    c64_speed_window.cpp \
    color_button.cpp \
    command_line_class.cpp \
    cartridge_class.cpp \
    cartridge_new_easyflash_window.cpp \
    cartridge_window.cpp \
    custom_splashscreen.cpp \
    d64_class.cpp \
    debugger_iec_window.cpp \
    debugger_vic_window.cpp \
    debugger_window.cpp \
    floppy1541_class.cpp \
    floppy_new_d64_window.cpp \
    floppy_window.cpp \
    georam_class.cpp \
    info_window.cpp \
    label_widget_mod.cpp \
    main.cpp \
    main_window.cpp \
    memory_window.cpp \
    mmu_class.cpp \
    mos6502_class.cpp \
    mos6510_class.cpp \
    mos6522_class.cpp \
    mos6526_class.cpp \
    mos6526_port.cpp \
    mos6569_class.cpp \
    mos6581_8085_class.cpp \
    new_romset_window.cpp \
    oscilloscope_widget.cpp \
    oscilloscope_window.cpp \
    reu_class.cpp \
    savepng.c \
    setup_window.cpp \
    show_c64_key_mapping_window.cpp \
    sid_dump_window.cpp \
    siddump.cpp \
    single_application.cpp \
    tape1530_class.cpp \
    tape_new_window.cpp \
    tape_window.cpp \
    tree_widget_mod.cpp \
    user_palette_window.cpp \
    video_crt_setup_window.cpp \
    vcd_class.cpp \
    video_capture_class.cpp \
    video_capture_window.cpp \
    video_crt_class.cpp \
    widget_bit_leiste.cpp \
    widget_file_browse.cpp \
    widget_floppy_status.cpp \
     widget_led.cpp \
    widget_memory_zeile.cpp \
    widget_sr_edit.cpp \
    widget_tape_counter.cpp

HEADERS += \
    am29f040_class.h \
    button_mod.h \
    c64_class.h \
    c64_colors.h \
    c64_file_types.h \
    c64_keyboard_window.h \
    c64_speed_window.h \
    color_button.h \
    command_line_class.h \
    cartridge_class.h \
    cartridge_new_easyflash_window.h \
    cartridge_window.h \
    custom_save_file_dialog.h \
    custom_splashscreen.h \
    d64_class.h \
    debugger_iec_window.h \
    debugger_vic_window.h \
    debugger_window.h \
    emu64_commands.h \
    floppy1541_class.h \
    floppy_new_d64_window.h \
    floppy_window.h \
    georam_class.h \
    info_window.h \
    label_widget_mod.h \
    main_window.h \
    memory_window.h \
    micro_code_tbl_6502.h \
    micro_code_tbl_6510.h \
    mmu_class.h \
    mos6502_class.h \
    mos6510_class.h \
    mos6522_class.h \
    mos6526_class.h \
    mos6526_port.h \
    mos6569_class.h \
    mos6581_8085_class.h \
    new_romset_window.h \
    oscilloscope_widget.h \
    oscilloscope_window.h \
    reu_class.h \
    savepng.h \
    setup_window.h \
    show_c64_key_mapping_window.h \
    sid_dump_window.h \
    siddump.h \
    single_application.h \
    structs.h \
    tape1530_class.h \
    tape_new_window.h \
    tape_window.h \
    tree_widget_mod.h \
    user_palette_window.h \
    utils.h \
    video_crt_setup_window.h \
    vcd_class.h \
    video_capture_class.h \
    video_capture_window.h \
    video_crt_class.h \
    widget_bit_leiste.h \
    widget_file_browse.h \
    widget_floppy_status.h \
    widget_led.h \
    widget_memory_zeile.h \
    widget_sr_edit.h \
    widget_tape_counter.h

FORMS += \
    c64_keyboard_window.ui \
    c64_speed_window.ui \
    cartridge_new_easyflash_window.ui \
    cartridge_window.ui \
    color_button.ui \
    debugger_iec_window.ui \
    debugger_vic_window.ui \
    debugger_window.ui \
    floppy_new_d64_window.ui \
    floppy_window.ui \
    info_window.ui \
    main_window.ui \
    memory_window.ui \
    new_romset_window.ui \
    oscilloscope_widget.ui \
    oscilloscope_window.ui \
    setup_window.ui \
    show_c64_key_mapping_window.ui \
    sid_dump_window.ui \
    tape_new_window.ui \
    tape_window.ui \
    user_palette_window.ui \
    video_crt_setup_window.ui \
    video_capture_window.ui \
    widget_bit_leiste.ui \
    widget_file_browse.ui \
    widget_floppy_status.ui \
    widget_led.ui \
    widget_memory_zeile.ui \
    widget_sr_edit.ui \
    widget_tape_counter.ui

TRANSLATIONS += emu64_en.ts \
                emu64_de.ts

RESOURCES += emu64.qrc
RC_FILE += emu64.rc

# Installation

message(Installpath: $$PREFIX)
DEFINES += DATA_PATH=\\\"$$PREFIX\\\"

win32 {
    target.path = $$PREFIX
    roms.path = $$PREFIX/roms
    floppy_sounds.path = $$PREFIX/floppy_sounds
    gfx.path = $$PREFIX/gfx
    txt.path = $$PREFIX
    languages.path = $$PREFIX/languages
} else {
    target.path = $$PREFIX/bin
    roms.path = $$PREFIX/share/$$TARGET/roms
    floppy_sounds.path = $$PREFIX/share/$$TARGET/floppy_sounds
    gfx.path = $$PREFIX/share/$$TARGET/gfx
    txt.path = $$PREFIX/share/doc/$$TARGET
    languages.path = $$PREFIX/share/$$TARGET/languages
    desktop.path = $$PREFIX/share/applications
    icons.path = $$PREFIX/share/icons/hicolor/64x64/apps
}

# C64 Roms
roms.files += ../roms/*

# Floppy Sounds
floppy_sounds.files = ../floppy_sounds/*

# GFX
gfx.files += ../grafik/kreis0.png
gfx.files += ../grafik/kreis1.png
gfx.files += ../grafik/pfeil0.png
gfx.files += ../grafik/pfeil1.png
gfx.files += ../grafik/sdl_icon.png

# TXT
txt.files += ../kommandozeilenparameter.txt
txt.files += ../hotkeys.txt
txt.files += ../LICENSE
txt.files += ../änderungen.txt

# Languages
languages.files += ../grafik/flaggen/emu64_de.png
languages.files += ../grafik/flaggen/emu64_en.png

# Linux Install Startmenü Eintrag
## /usr/local/share/icons/hicolor/64x64/apps/
linux
{
    desktop.files += emu64.desktop
    icons.files += ../grafik/emu64.png
}

## Momentane Lösung für MXE Build. MXE packt die *.qm Files momentan mit ins build/src/release Verzeichnis und nicht ins .qm Verzeichnis
win32 {
    languages.files += emu64_de.qm
    languages.files += emu64_en.qm
} else {
    languages.extra += $(INSTALL_FILE) .qm/emu64_de.qm $(INSTALL_ROOT)$$languages.path;
    languages.extra += $(INSTALL_FILE) .qm/emu64_en.qm $(INSTALL_ROOT)$$languages.path;
}

INSTALLS += target roms floppy_sounds gfx txt languages icons desktop
}

DISTFILES += \
    ../crossbuild-win-releases.sh
