package com.fulongbin.decoder;

import android.content.Context;
import android.text.TextUtils;
import android.util.Log;

import java.io.File;

public class Silk {
    final static String TAG = "Silk_v3_decoder";
    static {
        System.loadLibrary("silk_v3_decoder");
        if(App.INSTANCE!=null){
            cacheDir = App.INSTANCE.getCacheDir().getAbsolutePath();
        }
    }

    static String cacheDir;

    public static void setCacheDir(String dir){
        cacheDir = dir;
    }

    public static String getCacheDir() {
        return cacheDir;
    }

    public static boolean checkCacheDir(){
        if (cacheDir == null){
            Log.e(TAG,"cacheDir is null , please setCacheDir");
            return false;
        }
        File fCacheDir = new File(cacheDir);

        if(!fCacheDir.canWrite()){
            Log.e(TAG,"cacheDir["+cacheDir+"]  cat not write please set anther");
        }

        return true;
    }

    public static String createPcmFileName(String src){
        int index = src.lastIndexOf('/');
        String originName = src.substring(index,src.length());
        String temp = cacheDir+originName+".pcm";
        Log.e(TAG,"pmc temp "+temp);
        return temp;
    }

    public static boolean convertMp3ToSilk(String src, String dest){
        if(TextUtils.isEmpty(src)||TextUtils.isEmpty(dest)){
            Log.e(TAG,"src or dest is empty");
            return false;
        }
        if (!checkCacheDir()) {
            return false;
        }

        String temp = createPcmFileName(src);
        return mp3ToSilk(src,dest,temp)==0;
    }
    public static boolean convertSilkToMp3(String src, String dest){
        if(TextUtils.isEmpty(src)||TextUtils.isEmpty(dest)){
            Log.e(TAG,"src or dest is empty");
            return false;
        }
        if (!checkCacheDir()) {
            return false;
        }
        String temp = createPcmFileName(src);

        return silkToMp3(src,dest,temp)==0;
    }

    public static native int mp3ToSilk(String src, String dest, String temp);
    public static native int silkToMp3(String src, String dest, String temp);
}
