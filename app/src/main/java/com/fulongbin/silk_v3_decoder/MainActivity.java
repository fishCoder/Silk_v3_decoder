package com.fulongbin.silk_v3_decoder;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Spinner;
import android.widget.Toast;

import com.fulongbin.decoder.Silk;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {



    String cacheMp3 = "";
    String cacheWav = "";
    int defaultRate = 24000;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final Context context = this;

        Spinner spinner = findViewById(R.id.spinner);
        spinner.setSelection(3);
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                int rate = Integer.parseInt(parent.getItemAtPosition(position).toString());
                defaultRate = rate;
                Log.d("采样率", ""+rate);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        File fmp3 = new File(getCacheDir(),"hello.mp3");
        if(fmp3.exists()){
            cacheMp3 = fmp3.getAbsolutePath();
        }

        File fwav = new File(getCacheDir(),"hello.wav");
        if (fwav.exists()){
            cacheWav = fwav.getAbsolutePath();
        }

        findViewById(R.id.btn_silk_to_mp3).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                File srcFile = copyToCache();
                Log.d("当前采样率", ""+defaultRate);
                String dst = context.getCacheDir()+"/hello.mp3";
                //该步骤很耗时请勿在主线程执行
                boolean result = Silk.convertSilkToMp3(srcFile.getAbsolutePath(),dst,defaultRate);
                Toast.makeText(context,result?"转换成功":"转换失败",Toast.LENGTH_LONG).show();

                cacheMp3 = dst;

                play(cacheMp3);
            }
        });


        findViewById(R.id.btn_mp3_to_silk).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(TextUtils.isEmpty(cacheMp3)){
                    Toast.makeText(context,"请先按上面👆按钮",Toast.LENGTH_LONG).show();
                    return;
                }
                Log.d("当前采样率", ""+defaultRate);
                final String dst = context.getCacheDir()+"/hello.silk";
                //该步骤很耗时请勿在主线程执行
                boolean result = Silk.convertMp3ToSilk(cacheMp3,dst,defaultRate);
                Toast.makeText(context,result?"转换成功":"转换失败",Toast.LENGTH_LONG).show();



            }
        });


        findViewById(R.id.btn_silk_to_wav).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                File srcFile = copyToCache();
                Log.d("当前采样率", ""+defaultRate);
                String dst = context.getCacheDir()+"/hello.wav";
                //该步骤很耗时请勿在主线程执行
                boolean result = Silk.convertSilkToWav(srcFile.getAbsolutePath(),dst,defaultRate);
                Toast.makeText(context,result?"转换成功":"转换失败",Toast.LENGTH_LONG).show();

                cacheWav = dst;

                play(cacheWav);
            }
        });


        findViewById(R.id.btn_wav_to_silk).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(TextUtils.isEmpty(cacheWav)){
                    Toast.makeText(context,"请先按上面👆按钮",Toast.LENGTH_LONG).show();
                    return;
                }
                Log.d("当前采样率", ""+defaultRate);
                final String dst = context.getCacheDir()+"/hello.silk";
                //该步骤很耗时请勿在主线程执行
                boolean result = Silk.convertWavToSilk(cacheWav,dst,defaultRate);
                Toast.makeText(context,result?"转换成功":"转换失败",Toast.LENGTH_LONG).show();

                playPcm();

            }
        });







        findViewById(R.id.btn_clear_cache).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                File[] files = getCacheDir().listFiles();
                for (File file : files){
                    file.delete();
                }
            }
        });
    }


    void play(final String src){

        MediaExtractor mex = new MediaExtractor();
        try {
            mex.setDataSource(src);
            MediaFormat mf = mex.getTrackFormat(0);

            int bitRate = mf.getInteger(MediaFormat.KEY_BIT_RATE);
            int sampleRate = mf.getInteger(MediaFormat.KEY_SAMPLE_RATE);

            Log.d("bitRate", "" + bitRate);
            Log.d("sampleRate", "" + sampleRate);
        } catch (IOException e) {
            e.printStackTrace();
        }

        try {
            final MediaPlayer mediaPlayer = new MediaPlayer();
            mediaPlayer.setDataSource(src);
            mediaPlayer.prepareAsync();
            mediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
                @Override
                public void onPrepared(MediaPlayer mediaPlayer) {
                    mediaPlayer.start();
                }
            });
        } catch (IOException e) {
            e.printStackTrace();
        }


    }


    void  playPcm(){
        try {
            InputStream is = new FileInputStream(new File(cacheMp3+".pcm"));
            ByteArrayOutputStream bos = new ByteArrayOutputStream(10240);
            for (int b; (b = is.read()) != -1;) {
                bos.write(b);
            }
            byte[] audioData = bos.toByteArray();
            AudioTrack audioTrack= new AudioTrack(AudioManager.STREAM_MUSIC, 16000, AudioFormat.CHANNEL_OUT_MONO,AudioFormat.ENCODING_PCM_16BIT, audioData.length, AudioTrack.MODE_STREAM);
            audioTrack.write(audioData, 0, audioData.length);
            audioTrack.play();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    File copyToCache(){
        try {
            File srcFile = new File(getCacheDir(),"hello.silk");

            if(!srcFile.exists()){
                InputStream is = getAssets().open("hello.silk");
                //srcFile = new File(context.getCacheDir(),"hello.silk");
                FileOutputStream fos = new FileOutputStream(srcFile);
                int byteCount;
                byte[] buffer = new byte[1024*4];
                while((byteCount=is.read(buffer))!=-1) {
                    fos.write(buffer, 0, byteCount);
                }
                fos.flush();
                is.close();
                fos.close();
            }

            return srcFile;
        } catch (IOException e) {
            e.printStackTrace();
        }

        return null;
    }
}
