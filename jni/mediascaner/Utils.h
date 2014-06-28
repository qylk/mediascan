#ifndef MYUTILS_H_

#define MYUTILS_H_

#include <stdint.h>
#include <android/log.h>
#define NELEM(x) ((int)sizeof(x)/sizeof(x[0]))

uint16_t U16_AT(const uint8_t *ptr);
uint32_t U32_AT(const uint8_t *ptr);
uint64_t U64_AT(const uint8_t *ptr);

uint16_t U16LE_AT(const uint8_t *ptr);
uint32_t U32LE_AT(const uint8_t *ptr);
uint64_t U64LE_AT(const uint8_t *ptr);

uint64_t ntoh64(uint64_t x);
uint64_t hton64(uint64_t x);

#define TAG    "MP3ID3" // ������Զ����LOG�ı�ʶ
#define MLOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__) // ����LOGD����
#define MLOGI(...)  __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__) // ����LOGI����
#define MLOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,TAG,__VA_ARGS__) // ����LOGV����
#define MLOGW(...)  __android_log_print(ANDROID_LOG_WARN,TAG,__VA_ARGS__) // ����LOGW����
#define MLOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__) // ����LOGE����
#define MLOGF(...)  __android_log_print(ANDROID_LOG_FATAL,TAG,__VA_ARGS__) // ����LOGF����
//for debug
#define MASSERT(x)  if ( !(x)) { __android_log_assert( "assertion failed", TAG, "assert in '%s' at %d", __FILE__, __LINE__ ); }

#endif  // MYUTILS_H_
