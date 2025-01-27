LOCAL_PATH := $(call my-dir)
SOURCE_PATH := ./src
INCLUDE_PATH := ./include
PNG_SOURCE_PATH := ./png
PNG_INCLUDE_PATH := ./png/include
ZLIB_SOURCE_PATH := ./zlib

include $(CLEAR_VARS)
LOCAL_MODULE     := zlib
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(ZLIB_SOURCE_PATH)
LS_C             := $(subst $(LOCAL_PATH)/$(ZLIB_SOURCE_PATH),$(ZLIB_SOURCE_PATH),$(wildcard $(LOCAL_PATH)/$(ZLIB_SOURCE_PATH)/*.c))
LOCAL_SRC_FILES  := $(call LS_C,$(LOCAL_PATH))
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := png16
LOCAL_SRC_FILES := $(PNG_SOURCE_PATH)/lib/libpng16.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE     := Arkanoid
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(INCLUDE_PATH) $(LOCAL_PATH)/$(PNG_INCLUDE_PATH)
LS_CPP           := $(subst $(LOCAL_PATH)/$(SOURCE_PATH),$(SOURCE_PATH),$(wildcard $(LOCAL_PATH)/$(SOURCE_PATH)/*.cpp))
LOCAL_SRC_FILES  := $(call LS_CPP,$(LOCAL_PATH))
LOCAL_LDLIBS     := -llog -ldl -lz -landroid -lEGL -lGLESv2 -lOpenSLES
LOCAL_STATIC_LIBRARIES += png16 zlib
include $(BUILD_SHARED_LIBRARY)
