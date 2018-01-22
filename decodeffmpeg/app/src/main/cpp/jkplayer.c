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

    AVPacket *packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    AVFrame *frame = av_frame_alloc();
    AVFrame *pFrameYUV = av_frame_alloc();

    uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,pCodecCtx->width,pCodecCtx->height));
    avpicture_fill((AVPicture*)pFrameYUV,out_buffer,AV_PIX_FMT_YUV420P,pCodecCtx->width,pCodecCtx->height);

    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,pCodecCtx->height,pCodecCtx->pix_fmt,pCodecCtx->width,pCodecCtx->height,AV_PIX_FMT_YUV420P,SWS_BICUBIC,NULL,NULL,NULL);

    int got_picture,ret;
    FILE* fp_yuv = fopen(outputStr,"wb+");
    int frame_count = 0;
    while(av_read_frame(pFormatCtx,packet) >= 0) {
        if(packet->stream_index == v_stream_index) {
            if(avcodec_decode_video2(pCodecCtx,frame,&got_picture,packet) < 0) {
                LOGE("%s","解码失败");
                return;
            }
            if(got_picture) {
                sws_scale(sws_ctx, frame->data, frame->linesize, 0, pCodecCtx->height,
                          pFrameYUV->data, pFrameYUV->linesize);

                //输出到YUV文件
                //AVFrame像素帧写入文件
                //data解码后的图像像素数据（音频采样数据）
                //Y 亮度 UV 色度（压缩了） 人对亮度更加敏感
                //U V 个数是Y的1/4
                int y_size = pCodecCtx->width * pCodecCtx->height;
                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);
                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);
                fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);

                frame_count++;
                LOGI("解码第%d帧",frame_count);

            }

        }
        av_free_packet(packet);
    }

    fclose(fp_yuv);
    av_frame_free(&frame);
    avcodec_close(pCodecCtx);
    avformat_free_context(pFormatCtx);



    (*env)->ReleaseStringUTFChars(env, inputStr_, inputStr);
    (*env)->ReleaseStringUTFChars(env, outputStr_, outputStr);
}