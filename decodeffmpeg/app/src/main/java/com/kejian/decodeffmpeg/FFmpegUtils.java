package com.kejian.decodeffmpeg;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;
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

    public native void decodeVideo(String inputStr,String outputStr);

    public native void renderVideo(String inputStr,Surface surface);

    public native void playAudio(String input);

    /**
     * 创建一个AudioTrac对象，用于播放
     *
     * @return
     */
    public AudioTrack createAudioTrack(int sampleRateInHz, int nb_channels) {
        //固定格式的音频码流
        int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        Log.i("jason", "nb_channels:" + nb_channels);
        //声道布局
        int channelConfig;
        if (nb_channels == 1) {
            channelConfig = android.media.AudioFormat.CHANNEL_OUT_MONO;
        } else if (nb_channels == 2) {
            channelConfig = android.media.AudioFormat.CHANNEL_OUT_STEREO;
        } else {
            channelConfig = android.media.AudioFormat.CHANNEL_OUT_STEREO;
        }

        int bufferSizeInBytes = AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat);

        AudioTrack audioTrack = new AudioTrack(
                AudioManager.STREAM_MUSIC,
                sampleRateInHz, channelConfig,
                audioFormat,
                bufferSizeInBytes, AudioTrack.MODE_STREAM);
        //播放
        //audioTrack.play();
        //写入PCM
        //audioTrack.write(audioData, offsetInBytes, sizeInBytes);
        //播放完调用stop即可

        audioTrack.stop();
        return audioTrack;
    }

    public native void decodeAudio(String inputStr,String outputStr);

}
