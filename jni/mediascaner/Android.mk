LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES:=Z:/android4.0/frameworks/base/include \
		Z:/android4.0/system/core/include 

LOCAL_SRC_FILES := \
	com_qylk_app_provider_media_utils_MediaScaner.cpp\
	ID3.cpp\
	Utils.cpp\
	MetaData.cpp\
	MediaScannerClient.cpp\
	MediaScanner.cpp\
	MediaExtractor.cpp\
	Mp3Extractor.cpp\
	utfconv.cpp\
	DataBase.cpp\
	MpegUtils.cpp\
	XINGSeeker.cpp\
	VBRISeeker.cpp\
	FilesCache.cpp
	
#LOCAL_SRC_FILES := \
#	test.cpp\
#	ID3.cpp\
#	Utils.cpp\
#	MetaData.cpp\
#	MediaScannerClient.cpp\
#	MediaScanner.cpp\
#	MediaExtractor.cpp\
#	Mp3Extractor.cpp

LOCAL_MODULE := mediascaner
#LOCAL_SHARED_LIBRARIES := libiconv
LOCAL_LDLIBS    := -llog -lutils -lstagefright -lsqlite
#LOCAL_LDLIBS    := -lm -llog -lutils -lstagefright -lsqlite -lstdc++
include $(BUILD_SHARED_LIBRARY)
