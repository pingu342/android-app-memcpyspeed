LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE     := memcpyspeed
LOCAL_ARM_MODE   := arm
LOCAL_ARM_NEON   := true
LOCAL_SRC_FILES  := memcpyspeed.c
LOCAL_LDLIBS     := -llog
include $(BUILD_SHARED_LIBRARY)
