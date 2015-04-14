LOCAL_PATH := $(call my-dir)
SOURCE_PATH := ./src
INCLUDE_PATH := ./include

include $(CLEAR_VARS)

LOCAL_MODULE     := Arkanoid
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(INCLUDE_PATH)
LS_CPP           := $(subst $(LOCAL_PATH)/$(SOURCE_PATH),$(SOURCE_PATH),$(wildcard $(LOCAL_PATH)/$(SOURCE_PATH)/*.cpp))
LOCAL_SRC_FILES  := $(call LS_CPP,$(LOCAL_PATH))

LOCAL_LDLIBS     := -llog -ldl -landroid -lEGL -lGLESv2

include $(BUILD_SHARED_LIBRARY)
