#include <jni.h>



#include <android/log.h>

#include "libavutil/time.h"
#include <unistd.h>
//编码
#include "libavcodec/avcodec.h"
//封装格式处理
#include "libavformat/avformat.h"
//像素处理
#include "libswscale/swscale.h"

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <libavutil/imgutils.h>
#include "libyuv.h"


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
#include <time.h>
JNIEXPORT void JNICALL
Java_com_kejian_decodeffmpeg_FFmpegUtils_renderVideo_1(JNIEnv *env, jclass type, jstring inputStr_,jobject surface) {
    const char *inputStr = (*env)->GetStringUTFChars(env, inputStr_, 0);
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
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();

//    uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,pCodecCtx->width,pCodecCtx->height));
//    avpicture_fill((AVPicture*)pFrameYUV,out_buffer,AV_PIX_FMT_YUV420P,pCodecCtx->width,pCodecCtx->height);
//
//    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,pCodecCtx->height,pCodecCtx->pix_fmt,pCodecCtx->width,pCodecCtx->height,AV_PIX_FMT_YUV420P,SWS_BICUBIC,NULL,NULL,NULL);
//窗体
    ANativeWindow *pWindow = ANativeWindow_fromSurface(env, surface);
    //绘制时的缓冲区
    ANativeWindow_Buffer out_buffer;
    int got_picture,ret;
//    FILE* fp_yuv = fopen(outputStr,"wb+");
    int frame_count = 0;
    while(av_read_frame(pFormatCtx,packet) >= 0) {
        if(packet->stream_index == v_stream_index) {
            if(avcodec_decode_video2(pCodecCtx,yuv_frame,&got_picture,packet) < 0) {
                LOGE("%s","解码失败");
                return;
            }
            if(got_picture) {
//                sws_scale(sws_ctx, frame->data, frame->linesize, 0, pCodecCtx->height,
//                          pFrameYUV->data, pFrameYUV->linesize);
//
//                //输出到YUV文件
//                //AVFrame像素帧写入文件
//                //data解码后的图像像素数据（音频采样数据）
//                //Y 亮度 UV 色度（压缩了） 人对亮度更加敏感
//                //U V 个数是Y的1/4
//                int y_size = pCodecCtx->width * pCodecCtx->height;
//                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);
//                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);
//                fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);
                ANativeWindow_setBuffersGeometry(pWindow, pCodecCtx->width, pCodecCtx->height,
                                                 WINDOW_FORMAT_RGBA_8888);
                ANativeWindow_lock(pWindow, &out_buffer, NULL);

                //2、fix buffer

                //初始化缓冲区
                //设置属性，像素格式、宽高
                //rgb_frame的缓冲区就是Window的缓冲区，同一个，解锁的时候就会进行绘制
                avpicture_fill((AVPicture *) rgb_frame, out_buffer.bits, AV_PIX_FMT_RGBA,
                               pCodecCtx->width,
                               pCodecCtx->height);

                //YUV格式的数据转换成RGBA 8888格式的数据
                //FFmpeg可以转，但是会有问题，因此我们使用libyuv这个库来做
                //https://chromium.googlesource.com/external/libyuv
                //参数分别是数据、对应一行的大小
                //I420ToARGB(yuv_frame->data[0], yuv_frame->linesize[0],
                //           yuv_frame->data[1], yuv_frame->linesize[1],
                //           yuv_frame->data[2], yuv_frame->linesize[2],
                //           rgb_frame->data[0], rgb_frame->linesize[0],
                //           pCodecCtx->width, pCodecCtx->height);

                I420ToARGB(yuv_frame->data[0], yuv_frame->linesize[0],
                           yuv_frame->data[2], yuv_frame->linesize[2],
                           yuv_frame->data[1], yuv_frame->linesize[1],
                           rgb_frame->data[0], rgb_frame->linesize[0],
                           pCodecCtx->width, pCodecCtx->height);

                //3、unlock window
                ANativeWindow_unlockAndPost(pWindow);
                usleep(1000*16);
                frame_count++;
                LOGI("解码第%d帧",frame_count);

            }

        }
        av_free_packet(packet);
    }

    av_frame_free(&yuv_frame);
    av_frame_free(&rgb_frame);
    avcodec_close(pCodecCtx);
    avformat_free_context(pFormatCtx);

    (*env)->ReleaseStringUTFChars(env, inputStr_, inputStr);
}


#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
//能正确播放出来
JNIEXPORT jint JNICALL Java_com_kejian_decodeffmpeg_FFmpegUtils_renderVideo
        (JNIEnv * env, jclass clazz, jstring inputStr_,jobject surface)
{
    LOGI("play");

    // sd卡中的视频文件地址,可自行修改或者通过jni传入
//    char * file_name = "/sdcard/Video/00.flv";
    const char *file_name = (*env)->GetStringUTFChars(env, inputStr_, 0);

    av_register_all();

    AVFormatContext * pFormatCtx = avformat_alloc_context();

    // Open video file
    if(avformat_open_input(&pFormatCtx, file_name, NULL, NULL)!=0) {

        LOGE("Couldn't open file:%s\n", file_name);
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0) {
        LOGE("Couldn't find stream information.");
        return -1;
    }

    // Find the first video stream
    int videoStream = -1, i;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO
            && videoStream < 0) {
            videoStream = i;
        }
    }
    if(videoStream==-1) {
        LOGE("Didn't find a video stream.");
        return -1; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    AVCodecContext  * pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    AVCodec * pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {
        LOGE("Codec not found.");
        return -1; // Codec not found
    }

    if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return -1; // Could not open codec
    }

    // 获取native window
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);

    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;

    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow,  videoWidth, videoHeight, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0) {
        LOGE("Could not open codec.");
        return -1; // Could not open codec
    }

    // Allocate video frame
    AVFrame * pFrame = av_frame_alloc();

    // 用于渲染
    AVFrame * pFrameRGBA = av_frame_alloc();
    if(pFrameRGBA == NULL || pFrame == NULL) {
        LOGE("Could not allocate video frame.");
        return -1;
    }

    // Determine required buffer size and allocate buffer
    int numBytes=av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height, 1);
    uint8_t * buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    int frameFinished;
    AVPacket packet;
    while(av_read_frame(pFormatCtx, &packet)>=0) {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) {

            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // 并不是decode一次就可解码出一帧
            if (frameFinished) {

                // lock native window buffer
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

                // 格式转换
                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                // 获取stride
                uint8_t * dst = windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t * src = (uint8_t*) (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
                usleep(1000*36);
            }

        }
        av_packet_unref(&packet);
    }

    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);
    return 0;
}

