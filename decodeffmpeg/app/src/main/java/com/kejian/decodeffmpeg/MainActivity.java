package com.kejian.decodeffmpeg;

import android.Manifest;
import android.os.Build;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final String intput = Environment.getExternalStorageDirectory().getAbsolutePath()+ File.separator+"input.mp4";
        final String output = Environment.getExternalStorageDirectory().getAbsolutePath()+ File.separator+"sintel_decoded_22.yuv";
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.MOUNT_UNMOUNT_FILESYSTEMS}, 0);
        }

        Button btn_decode = (Button) findViewById(R.id.btn_decode);
        btn_decode.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FFmpegUtils.decodeVideo(intput,output);
            }
        });

        final SurfaceView sv_video = (SurfaceView)findViewById(R.id.sv_video);
        Button btn_render = (Button) findViewById(R.id.btn_render);
        btn_render.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FFmpegUtils.renderVideo(intput,sv_video.getHolder().getSurface());
            }
        });
    }
}
