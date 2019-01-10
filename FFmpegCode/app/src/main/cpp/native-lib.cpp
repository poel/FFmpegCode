#include <jni.h>
#include <string>

#include <android/log.h>

extern "C" {
#include <libavformat/avformat.h>
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"

//像素处理
#include "libswscale/swscale.h"
#include <android/native_window_jni.h>
#include <unistd.h>
}

extern "C" JNIEXPORT jstring JNICALL
Java_cc_gl_com_ffmpegcode_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_cc_gl_com_ffmpegcode_MainActivity_helloNDK(JNIEnv *env, jobject) {
    char *chello = new char[200];
    __android_log_print(ANDROID_LOG_ERROR, "tag", "c : %s", chello);
    __android_log_print(ANDROID_LOG_ERROR, "tag", "编码器配置： %s", avcodec_configuration());
    char *newstr = strcat(chello, avcodec_configuration());

    //将java传过来的字符串和编码器配置信息拼接起来并返回去
    return env->NewStringUTF(newstr);
}

extern "C" JNIEXPORT jstring JNICALL
Java_cc_gl_com_ffmpegcode_MainActivity_getInfo(JNIEnv *env, jobject) {

    char info[40000] = {0};
    av_register_all();
    AVCodec *c_temp = av_codec_next(NULL);
    while(c_temp != NULL){
        if(c_temp->decode!=NULL){
            sprintf(info,"%s[Dec]",info);
        }else{
            sprintf(info,"%s[Enc]",info);
        }
        switch(c_temp->type){
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info,"%s[Video]",info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info,"%s[Audio]",info);
                break;
            default:
                sprintf(info,"%s[Other]",info);
                break;
        }
        sprintf(info,"%s[%10s]\n",info,c_temp->name);
        c_temp=c_temp->next;
    }
    __android_log_print(ANDROID_LOG_INFO,"myTag","info:\n%s",info);

    return env->NewStringUTF(info);
}

#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"LC",FORMAT,##__VA_ARGS__);
extern "C" JNIEXPORT void JNICALL
Java_cc_gl_com_ffmpegcode_VideoView_render(JNIEnv *env,
                                           jobject obj,
                                           jstring inputStr_,
                                           jobject surface) {

    jboolean isCopy;
    const char *inputPath = env->GetStringUTFChars(inputStr_, &isCopy);

    //注册各大组件
    av_register_all();

    LOGE("注册成功")
    AVFormatContext *avFormatContext = avformat_alloc_context();//获取上下文
    int error;
    char buf[] = "";
    //打开视频地址并获取里面的内容(解封装)
    if (error = avformat_open_input(&avFormatContext, inputPath, NULL, NULL) < 0) {
        av_strerror(error, buf, 1024);
        // LOGE("%s" ,inputPath)
        LOGE("Couldn't open file %s: %d(%s)", inputPath, error, buf);
        // LOGE("%d",error)
        LOGE("打开视频失败")
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("获取内容失败")
        return;
    }
    //获取到整个内容过后找到里面的视频流
    int video_index=-1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            //如果是视频流,标记一哈
            video_index = i;
        }
    }
    LOGE("成功找到视频流")
    //对视频流进行解码
    //获取解码器上下文
    AVCodecContext *avCodecContext = avFormatContext->streams[video_index]->codec;
    //获取解码器
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    //打开解码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("打开失败")
        return;
    }

    //申请AVPacket
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_init_packet(packet);
    //申请AVFrame
    AVFrame *frame = av_frame_alloc();//分配一个AVFrame结构体,AVFrame结构体一般用于存储原始数据，指向解码后的原始帧
    AVFrame *rgb_frame = av_frame_alloc();//分配一个AVFrame结构体，指向存放转换成rgb后的帧
    //输出文件
    //FILE *fp = fopen(outputPath,"wb");

    //缓存区
    uint8_t  *out_buffer= (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_RGBA,
                                                                  avCodecContext->width,avCodecContext->height));
    //与缓存区相关联，设置rgb_frame缓存区
    avpicture_fill((AVPicture *)rgb_frame,out_buffer,AV_PIX_FMT_RGBA,avCodecContext->width,avCodecContext->height);


    SwsContext* swsContext = sws_getContext(avCodecContext->width,avCodecContext->height,avCodecContext->pix_fmt,
                                            avCodecContext->width,avCodecContext->height,AV_PIX_FMT_RGBA,
                                            SWS_BICUBIC,NULL,NULL,NULL);

    //取到nativewindow
    ANativeWindow *nativeWindow=ANativeWindow_fromSurface(env,surface);
    if(nativeWindow==0){
        LOGE("nativewindow取到失败")
        return;
    }
    //视频缓冲区
    ANativeWindow_Buffer native_outBuffer;

    int frameCount;
    int h =0;

    LOGE("解码 ")
    while (av_read_frame(avFormatContext, packet) >= 0) {
        //LOGE("解码 %d",packet->stream_index)
        //LOGE("VINDEX %d",video_index)
        if(packet->stream_index==video_index){
            //LOGE("解码 hhhhh")
            //如果是视频流
            //解码
            avcodec_decode_video2(avCodecContext, frame, &frameCount, packet);
            //LOGE("解码中....  %d",frameCount)
            if (frameCount) {
                //LOGE("转换并绘制")
                //说明有内容
                //绘制之前配置nativewindow
                ANativeWindow_setBuffersGeometry(nativeWindow,avCodecContext->width,avCodecContext->height,WINDOW_FORMAT_RGBA_8888);
                //上锁
                ANativeWindow_lock(nativeWindow, &native_outBuffer, NULL);
                //转换为rgb格式
                sws_scale(swsContext,(const uint8_t *const *)frame->data,frame->linesize,0,
                          frame->height,rgb_frame->data,
                          rgb_frame->linesize);
                //  rgb_frame是有画面数据
                uint8_t *dst= (uint8_t *) native_outBuffer.bits;

                //拿到一行有多少个字节 RGBA
                int destStride=native_outBuffer.stride * 4;

                //像素数据的首地址
                uint8_t * src=  rgb_frame->data[0];

                //实际内存一行数量
                int srcStride = rgb_frame->linesize[0];

                //int i=0;
                for (int i = 0; i < avCodecContext->height; ++i) {
                    //memcpy(void *dest, const void *src, size_t n)
                    //将rgb_frame中每一行的数据复制给nativewindow
                    memcpy(dst + i * destStride,  src + i * srcStride, srcStride);
                }

                //解锁
                ANativeWindow_unlockAndPost(nativeWindow);
                //usleep(1000 * 16);
            }
        }
        av_free_packet(packet);
    }

    //释放
    ANativeWindow_release(nativeWindow);
    av_frame_free(&frame);
    av_frame_free(&rgb_frame);
    avcodec_close(avCodecContext);
    avformat_free_context(avFormatContext);
    env->ReleaseStringUTFChars(inputStr_, inputPath);
}