LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := monitor
LOCAL_LDLIBS := -llog

#LOCAL_SRC_FILES := $(call all-subdir-c-files)
LOCAL_SRC_FILES := syscall_hook.c

include $(BUILD_EXECUTABLE)
