#include <jni.h>



#include <android/log.h>

#include "libavutil/time.h"
//编码
#include "libavcodec/avcodec.h"
//封装格式处理
#include "libavformat/avformat.h"
//像素处理
#include "libswscale/swscale.h"


#define TAG "kejian"
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,TAG,FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,TAG,FORMAT,##__VA_ARGS__);

JNIEXPORT void JNICALL
Java_com_kejian_decodeffmpeg_FFmpegUtils_decodeVideo(JNIEnv *env, jclass type, jstring inputStr_,
                                                     jstring outputStr_) {
    const char *inputStr = (*env)->GetStringUTFChars(env, inputStr_, 0);
    const char *outputStr = (*env)->GetStringUTFChars(env, outputStr_, 0);

    LOGI("视频的路劲是%s",inputStr);
    // TODO
    av_register_all();

    AVFormatContext * pFormatCtx = avformat_alloc_context();

    if(avformat_open_input(&pFormatCtx,inputStr,NULL,NULL) != 0) {
        LOGE("%s","无法打开输入视频");
        return;
    }

    if(avformat_find_stream_info(pFormatCtx,NULL) < 0) {
        LOGE("%s","无法获取视频文件的相关信息");
        return;
    }

    int v_stream_index = -1;
    int i = 0;
    for(;i<pFormatCtx->nb_streams;i++) {
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            v_stream_index = i;
            break;
        }
    }
    if(v_stream_index == -1) {
        LOGE("%s","找不到视频流索引");
        return;
    }

    AVCodecContext *pCodecCtx = pFormatCtx->streams[v_stream_index]->codec;
    AVCodec * pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL) {
        LOGE("%s","找不到解码器，或者视频已加密\n");
        return;
    }

    if(avcodec_open2(pCodecCtx,pCodec,NULL) < 0) {
        LOGE("%s","解码器打开失败");
        return;
    }

    LOGI("视频的文件格式%s",pFormatCtx->iformat->name);
    LOGI("视频时长：%lld",(pFormatCtx->duration)/1000000);
    LOGI("视频的宽高：%d,%d", pCodecCtx->width, pCodecCtx->height);
    LOGI("解码器的名称：%s", pCodec->name);


    (*env)->ReleaseStringUTFChars(env, inputStr_, inputStr);
    (*env)->ReleaseStringUTFChars(env, outputStr_, outputStr);
}