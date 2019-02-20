//
// Created by Steve Guan on 2019/1/14.
//

#ifndef FFMPEGCODE_NATIVE_LIB_H
#define FFMPEGCODE_NATIVE_LIB_H

// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#endif //FFMPEGCODE_NATIVE_LIB_H
