package com.kejian.decodeffmpeg;

import android.view.Surface;

/**
 * 项目名称：ffmpegkejian
 * 类描述：
 * 创建人：kejian
 * 创建时间：2018-01-11 9:43
 * 修改人：Administrator
 * 修改时间：2018-01-11 9:43
 * 修改备注：
 */
public class FFmpegUtils {
    static {
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("jkplayer-lib");
    }

    public native static  void decodeVideo(String inputStr,String outputStr);

    public native static void renderVideo(String inputStr,Surface surface);
}
