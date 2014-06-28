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

#define TAG    "MP3ID3" // 这个是自定义的LOG的标识
#define MLOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__) // 定义LOGD类型
#define MLOGI(...)  __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__) // 定义LOGI类型
#define MLOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,TAG,__VA_ARGS__) // 定义LOGV类型
#define MLOGW(...)  __android_log_print(ANDROID_LOG_WARN,TAG,__VA_ARGS__) // 定义LOGW类型
#define MLOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__) // 定义LOGE类型
#define MLOGF(...)  __android_log_print(ANDROID_LOG_FATAL,TAG,__VA_ARGS__) // 定义LOGF类型
//for debug
#define MASSERT(x)  if ( !(x)) { __android_log_assert( "assertion failed", TAG, "assert in '%s' at %d", __FILE__, __LINE__ ); }

#endif  // MYUTILS_H_
