package com.kejian.decodeffmpeg;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * 项目名称：decodeffmpeg
 * 类描述：
 * 创建人：kejian
 * 创建时间：2018-01-22 14:38
 * 修改人：Administrator
 * 修改时间：2018-01-22 14:38
 * 修改备注：
 */
public class VideoView extends SurfaceView {

    public VideoView(Context context) {
        this(context, null, 0);
    }

    public VideoView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public VideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        //初始化像素绘制的格式为RGBA_8888（色彩最丰富）
        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
    }

}
