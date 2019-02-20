//
// Created by Steve Guan on 2019/1/12.
//
#include <jni.h>
#include <string>

#include <android/log.h>
#include "native-lib.h"

extern "C" {
#include <libavformat/avformat.h>
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libswresample/swresample.h"
#include "libavutil/pixdesc.h"

//像素处理
#include "libswscale/swscale.h"
#include <android/native_window_jni.h>
#include <unistd.h>
}

#define TAG "JNITEST"

int interrupt_callback(void* param) {

}

extern "C" JNIEXPORT jstring JNICALL
Java_cc_gl_com_ffmpegcode_MainActivity_decodeVideo(JNIEnv *env,
                                               jobject object,
                                               jstring inputStr_) {
    int videoStremIndex = -1, audioStreamIndex = -1;
    AVStream *audioStream = NULL;
    AVStream *videoStream = NULL;

    jboolean isCopy;
    const char *inputPath = env->GetStringUTFChars(inputStr_, &isCopy);

    avformat_network_init();
    av_register_all();

    //获取上下文
    AVFormatContext *formatCtx = avformat_alloc_context();

    void* ptr;
    AVIOInterruptCB int_cb = {interrupt_callback, ptr};

    formatCtx->interrupt_callback = int_cb;

    avformat_open_input(&formatCtx, inputPath, NULL, NULL);

    avformat_find_stream_info(formatCtx, NULL);

    for(int i = 0; i < formatCtx->nb_streams; i++) {
        if(AVMEDIA_TYPE_VIDEO == formatCtx->streams[i]->codecpar->codec_type) {
            audioStream = formatCtx->streams[i];
            videoStremIndex = i;
        } else if(AVMEDIA_TYPE_AUDIO == formatCtx->streams[i]->codecpar->codec_type) {
            videoStream = formatCtx->streams[i];
            audioStreamIndex = i;
        }
    }

    AVCodecContext *audioCodecCtx = audioStream->codec;
    AVCodec *audioCodec = avcodec_find_decoder(audioCodecCtx->codec_id);

    if(!audioCodec) {
        //找不到对应的音频解码器
        LOGE("找不到对应的音频解码器!");
    }

    int openCodecErrCode = 0;
    if((openCodecErrCode == avcodec_open2(audioCodecCtx, audioCodec, NULL) < 0)) {
        LOGE("获取音频解码器失败!");
    }

    AVCodecContext *videoCodecCtx = videoStream->codec;
    AVCodec *videoCodec = avcodec_find_decoder(videoCodecCtx->codec_id);

    if(!videoCodec) {
        //找不到对应的音频解码器
        LOGE("找不到对应的音频解码器!");
    }

    int openVideoCodecErrCode = 0;
    if((openVideoCodecErrCode == avcodec_open2(videoCodecCtx, videoCodec, NULL) < 0)) {
        LOGE("获取音频解码器失败!");
    }

    /**
     * 初始化解码后数据的结构体
     * */
    SwrContext *swrContext = NULL;
}