//
// Created by ketian on 16-9-23.
//

#include <jni.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "libwav/wav.h"
#include "silk.h"
#include "lame.h"
#include "easy_mad.h"


JNIEXPORT jint JNICALL
Java_com_fulongbin_decoder_Silk_silkToMp3(JNIEnv *env, jclass clazz, jstring src, jstring dest,
                                          jstring tmp, jint rate) {
    const char *src_c = env->GetStringUTFChars(src, 0);
    const char *dest_c = env->GetStringUTFChars(dest, 0);

    const char *tmp_c = env->GetStringUTFChars(tmp, 0);

    LOGD("convert %s to %s", src_c, dest_c);


    FILE *pcm = fopen(tmp_c, "wb+");
    if (convertSilk2PCM(src_c, pcm, rate) != 0) {
        LOGD("convert silk to pcm failed");
        return -1;
    }
    fseek(pcm, 0, SEEK_SET);

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, rate);
    lame_set_out_samplerate(lame, rate);
    lame_set_num_channels(lame, 1);
    lame_set_brate(lame, 128);
    lame_set_mode(lame, MONO);
    lame_set_quality(lame, 5);
    lame_init_params(lame);


    FILE *mp3 = fopen(dest_c, "wb+");
    int read, write;

    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;
    short int pcm_buffer[PCM_SIZE];
    unsigned char mp3_buffer[MP3_SIZE];

    do {
        read = fread(pcm_buffer, sizeof(short int), PCM_SIZE, pcm);
        if (read == 0) {
            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
        } else {
            write = lame_encode_buffer(lame, pcm_buffer, NULL, read, mp3_buffer, MP3_SIZE);
        }

        fwrite(mp3_buffer, 1, write, mp3);
    } while (read != 0);

    lame_close(lame);
    fclose(mp3);
    fclose(pcm);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_fulongbin_decoder_Silk_mp3ToSilk(JNIEnv *env, jclass clazz, jstring src, jstring dest,
                                          jstring tmpUrl, jint rate) {
    const char *src_c = env->GetStringUTFChars(src, 0);
    const char *dest_c = env->GetStringUTFChars(dest, 0);
    const char *tmp = env->GetStringUTFChars(tmpUrl, 0);
    const int  rate_c = rate;

    LOGD("convert %s to %s", src_c, dest_c);


    if (convertMP32PCM(src_c, tmp) == -1) {
        LOGD("mp3_decode failed", tmp);
        return -1;
    }


    FILE *silk = fopen(dest_c, "wb+");
    if (convertPCM2Silk(tmp, silk, rate_c) != 0) {
        LOGD("convert pcm to silk failed");
        return -1;
    }

    fclose(silk);



    return 0;
}

JNIEXPORT jint JNICALL
Java_com_fulongbin_decoder_Silk_silkToWav(JNIEnv *env, jclass clazz, jstring src, jstring dest,
                                          jstring tmp, jint rate) {
    const char *src_c = env->GetStringUTFChars(src, 0);
    const char *dest_c = env->GetStringUTFChars(dest, 0);

    const char *tmp_c = env->GetStringUTFChars(tmp, 0);

    LOGD("convert %s to %s", src_c, dest_c);


    FILE *pcm = fopen(tmp_c, "wb+");
    if (convertSilk2PCM(src_c, pcm, rate) != 0) {
        LOGD("convert silk to pcm failed");
        return -1;
    }


    FILE *wav = fopen(dest_c, "wb+");

    if (convertPCM2WAV(pcm, wav, rate) != 0) {
        LOGD("convert pcm to wav failed");
        return -1;
    }

    fclose(wav);
    fclose(pcm);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_fulongbin_decoder_Silk_wavToSilk(JNIEnv *env, jclass clazz, jstring src, jstring dest,
                                          jstring tmpUrl, jint rate) {
    const char *src_c = env->GetStringUTFChars(src, 0);
    const char *dest_c = env->GetStringUTFChars(dest, 0);

    const char *tmp = env->GetStringUTFChars(tmpUrl, 0);

    LOGD("convert %s to %s", src_c, dest_c);

    FILE *wav = fopen(src_c, "rb");
    FILE *pcm = fopen(tmp,"wb+");
    if (convertWAV2PCM(wav, pcm) == -1) {
        LOGD("wav tp pcm failed", tmp);
        return -1;
    }


    FILE *silk = fopen(dest_c, "wb+");
    if (convertPCM2Silk(tmp, silk, rate) != 0) {
        LOGD("convert pcm to silk failed");
        return -1;
    }

    fclose(wav);
    fclose(pcm);
    fclose(silk);


    return 0;
}

#ifdef __cplusplus
}
#endif