package com.kejian.decodeffmpeg;

import android.Manifest;
import android.os.Build;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final String intput = Environment.getExternalStorageDirectory().getAbsolutePath()+ File.separator+"input.mp4";
        final String output = Environment.getExternalStorageDirectory().getAbsolutePath()+ File.separator+"sintel_decoded.yuv";
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
    }
}
