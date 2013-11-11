LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := emu64
LOCAL_SRC_FILES := emu64.cpp
LOCAL_SRC_FILES += asset_texture_class.cpp
LOCAL_SRC_FILES += text_box_class.cpp
LOCAL_SRC_FILES += dsp.cpp
LOCAL_SRC_FILES += c64_class.cpp

LOCAL_SRC_FILES += ../../../mos6581_8085_class.cpp

LOCAL_LDLIBS := -llog -landroid -lGLESv1_CM -lOpenSLES

include $(BUILD_SHARED_LIBRARY)
