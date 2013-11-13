LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := emu64
LOCAL_SRC_FILES := emu64.cpp
LOCAL_SRC_FILES += asset_texture_class.cpp
LOCAL_SRC_FILES += text_box_class.cpp
LOCAL_SRC_FILES += dsp.cpp
LOCAL_SRC_FILES += c64_class.cpp

LOCAL_SRC_FILES += ../../../mmu_class.cpp
LOCAL_SRC_FILES += ../../../crt_class.cpp
LOCAL_SRC_FILES += ../../../am29f040_class.cpp
LOCAL_SRC_FILES += ../../../mos6510_class.cpp
LOCAL_SRC_FILES += ../../../mos6569_class.cpp
LOCAL_SRC_FILES += ../../../mos6526_class.cpp
LOCAL_SRC_FILES += ../../../mos6526_port.cpp
LOCAL_SRC_FILES += ../../../mos6581_8085_class.cpp
LOCAL_SRC_FILES += ../../../floppy1541_class.cpp
LOCAL_SRC_FILES += ../../../mos6502_class.cpp
LOCAL_SRC_FILES += ../../../mos6522_class.cpp
LOCAL_SRC_FILES += ../../../siddump.cpp

LOCAL_LDLIBS := -llog -landroid -lGLESv1_CM -lOpenSLES

include $(BUILD_SHARED_LIBRARY)
