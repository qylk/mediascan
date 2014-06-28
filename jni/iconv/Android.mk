LOCAL_PATH:= $(call my-dir)
#libiconv.so
include $(CLEAR_VARS)
LOCAL_MODULE := iconv

LOCAL_CFLAGS := \
  -Wno-multichar \
  -DANDROID \
  -DLIBDIR="c" \
  -DBUILDING_LIBICONV \
  -DIN_LIBRARY

LOCAL_SRC_FILES := \
  libcharset/lib/localcharset.c \
  lib/iconv.c \
  lib/relocatable.c

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/include \
  $(LOCAL_PATH)/libcharset \
  $(LOCAL_PATH)/lib \
  $(LOCAL_PATH)/libcharset/include \
  $(LOCAL_PATH)/srclib
  
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)

#test
#include $(CLEAR_VARS)
#LOCAL_MODULE :=genutf8

#LOCAL_SRC_FILES := tests/genutf8.c
#LOCAL_C_INCLUDES += \
#  $(LOCAL_PATH)/include \
#  $(LOCAL_PATH)/libcharset \
#  $(LOCAL_PATH)/lib \
#  $(LOCAL_PATH)/libcharset/include \
#  $(LOCAL_PATH)/srclib
#LOCAL_SHARED_LIBRARIES := libiconv
#include $(BUILD_EXECUTABLE)