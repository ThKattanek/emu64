#-------------------------------------------------
#
# Project created by QtCreator 2011-03-20T09:30:27
# Emu64 by Thorsten Kattanek
#-------------------------------------------------

TRANSLATIONS = emu64_de.ts
TRANSLATIONS = emu64_en.ts

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

SOURCES += \
    main.cpp \
    info_window.cpp \
    main_window.cpp \
    siddump.cpp \
    c64_class.cpp \
    mos6569_class.cpp \
    mos6581_8085_class.cpp \
    mmu_class.cpp \
    mos6510_class.cpp \
    mos6526_class.cpp \
    mos6526_port.cpp \
    videopal_class.cpp \
    tv_setup_window.cpp \
    floppy_window.cpp \
    c64_keyboard_window.cpp \
    crt_class.cpp \
    am29f040_class.cpp \
    crt_window.cpp \
    widget_file_browse.cpp \
    tree_widget_mod.cpp \
    debugger_window.cpp \
    widget_sr_edit.cpp \
    label_widget_mod.cpp \
    mos6522_class.cpp \
    mos6502_class.cpp \
    floppy1541_class.cpp \
    widget_floppy_status.cpp \
    d64_class.cpp \
    memory_window.cpp \
    widget_memory_zeile.cpp \
    widget_bit_leiste.cpp \
    setup_window.cpp \
    widget_d64_file.cpp \
    debugger_vic_window.cpp \
    single_application.cpp \
    button_mod.cpp \
    custom_splashscreen.cpp \
    c64_speed_window.cpp

HEADERS  += \
    version.h \
    info_window.h \
    main_window.h \
    mos6581_8085_wellenformen.h \
    mos6581_8085_calc.h \
    siddump.h \
    mos6581_8085_voice.h \
    c64_class.h \
    mos6569_class.h \
    structs.h \
    mos6581_8085_class.h \
    mmu_class.h \
    mos6510_class.h \
    micro_code_tbl_6510.h \
    micro_code_string_tbl_6510.h \
    mos6526_class.h \
    mos6526_port.h \
    c64_colors.h \
    videopal_class.h \
    tv_setup_window.h \
    floppy_window.h \
    c64_keys.h \
    c64_keyboard_window.h \
    crt_class.h \
    am29f040_class.h \
    crt_window.h \
    widget_file_browse.h \
    tree_widget_mod.h \
    debugger_window.h \
    widget_sr_edit.h \
    label_widget_mod.h \
    mos6522_class.h \
    mos6502_class.h \
    micro_code_tbl_6502.h \
    micro_code_string_tbl_6502.h \
    floppy1541_class.h \
    widget_floppy_status.h \
    d64_class.h \
    cpu_info.h \
    memory_window.h \
    widget_memory_zeile.h \
    widget_bit_leiste.h \
    setup_window.h \
    widget_d64_file.h \
    debugger_vic_window.h \
    single_application.h \
    button_mod.h \
    custom_splashscreen.h \
    c64_speed_window.h

FORMS    += \
    info_window.ui \
    main_window.ui \
    widgedaudiovisuell.ui \
    tv_setup_window.ui \
    floppy_window.ui \
    c64_keyboard_window.ui \
    crt_window.ui \
    widget_file_browse.ui \
    debugger_window.ui \
    widget_sr_edit.ui \
    widget_floppy_status.ui \
    memory_window.ui \
    widget_memory_zeile.ui \
    widget_bit_leiste.ui \
    setup_window.ui \
    widget_d64_file.ui \
    debugger_vic_window.ui \
    c64_speed_window.ui

RESOURCES += \
    emu64.qrc

linux-g++-64{
TARGET = emu64
DESTDIR = "bin/linux_x86_64"
LIBS += -lSDL -lSDL_gfx -lSDL_image -lquazip -lGL -lGLU
}

linux-g++-32{
TARGET = emu64
DESTDIR = "bin/linux_i686"
LIBS += -lSDL -lSDL_gfx -lSDL_image -lquazip -lGL -lGLU
}

win32-g++-cross{
TARGET = emu64
RC_FILE = emu64.rc
DESTDIR = "bin/win_x32"
LIBS += -lSDL -lquazip -lopengl32 -lglu32 -lSDL_gfx -lSDL_image
}

win64-g++-cross{
TARGET = emu64
RC_FILE = emu64.rc
DESTDIR = "bin/win_x64"
LIBS += -lSDL -lquazip -lopengl32 -lglu32 -lSDL_gfx -lSDL_image
}

OTHER_FILES += \
    emu64.rc \
    docs/ToDo \
    ../bin/Creating Final Versions/Linux/readme.txt \
    docs/readme linux.txt \
    docs/readme linux.txt \
    docs/readme linux.txt \
    docs/readme.txt \
    fonts/lucon.ttf \
    fonts/emu64.ttf \
    roms/kernal.rom \
    roms/char.rom \
    roms/basic.rom \
    roms/1541.rom \
    readme.md \
    emu64_en.qm \
    emu64_de.ts \
    emu64_de.qm \
    emu64_en.ts \
    floppy_sounds/stepper_inc.raw \
    floppy_sounds/stepper_dec.raw \
    floppy_sounds/motor_on.raw \
    floppy_sounds/motor_off.raw \
    floppy_sounds/motor.raw \
    floppy_sounds/FloppySound Hinweis.txt \
    floppy_sounds/anschlag.raw \
    lizenz.pdf \
    license.pdf















































