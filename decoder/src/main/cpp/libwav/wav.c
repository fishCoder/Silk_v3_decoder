//
// Created by fulongbin on 2019/2/1.
//

#include "wav.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>





int convertWAV2PCM(FILE *fwav,FILE *fpcm){

    fseek(fwav,44,SEEK_SET);

    char data[1024*4];
    int read_size = 0;
    while ((read_size=fread(data,1, sizeof(data),fwav))>0){
        fwrite(data,1,read_size,fpcm);
    }

    return 0;

}

typedef struct {
    unsigned char   chunk_id[4];        // RIFF string
    unsigned int    chunk_size;         // overall size of file in bytes (36 + data_size)
    unsigned char   sub_chunk1_id[8];   // WAVEfmt string with trailing null char
    unsigned int    sub_chunk1_size;    // 16 for PCM.  This is the size of the rest of the Subchunk which follows this number.
    unsigned short  audio_format;       // format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
    unsigned short  num_channels;       // Mono = 1, Stereo = 2
    unsigned int    sample_rate;        // 8000, 16000, 44100, etc. (blocks per second)
    unsigned int    byte_rate;          // SampleRate * NumChannels * BitsPerSample/8
    unsigned short  block_align;        // NumChannels * BitsPerSample/8
    unsigned short  bits_per_sample;    // bits per sample, 8- 8bits, 16- 16 bits etc
    unsigned char   sub_chunk2_id[4];   // Contains the letters "data"
    unsigned int    sub_chunk2_size;    // NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
} wav_header_t;


char* dummy_get_raw_pcm(FILE *fp_pcm, int *bytes_read)
{
    long lSize;
    char *pcm_buf;
    size_t result;

    if (fp_pcm == NULL) {
        printf ("File error");
        return  -1;
    }

    // obtain file size:
    fseek (fp_pcm , 0 , SEEK_END);
    lSize = ftell (fp_pcm);
    rewind (fp_pcm);

    // allocate memory to contain the whole file:
    pcm_buf = (char*) malloc (sizeof(char) * lSize);
    if (pcm_buf == NULL) {
        printf ("Memory error");
        return  -1;
    }

    // copy the file into the pcm_buf:
    result = fread (pcm_buf, 1, lSize, fp_pcm);
    if (result != lSize) {
        printf ("Reading error");
        return  -1;
    }

    *bytes_read = (int) lSize;
    return pcm_buf;
}

void get_wav_header(int raw_sz, wav_header_t *wh)
{
    // RIFF chunk
    strcpy(wh->chunk_id, "RIFF");
    wh->chunk_size = 36 + raw_sz;

    // fmt sub-chunk (to be optimized)
    strncpy(wh->sub_chunk1_id, "WAVEfmt ", strlen("WAVEfmt "));
    wh->sub_chunk1_size = 16;
    wh->audio_format = 1;
    wh->num_channels = 1;
    wh->sample_rate = 24000;
    wh->bits_per_sample = 16;
    wh->block_align = wh->num_channels * wh->bits_per_sample / 8;
    wh->byte_rate = wh->sample_rate * wh->num_channels * wh->bits_per_sample / 8;

    // data sub-chunk
    strncpy(wh->sub_chunk2_id, "data", strlen("data"));
    wh->sub_chunk2_size = raw_sz;
}

void dump_wav_header (wav_header_t *wh)
{
    printf ("=========================================\n");
    printf ("chunk_id:\t\t\t%s\n", wh->chunk_id);
    printf ("chunk_size:\t\t\t%d\n", wh->chunk_size);
    printf ("sub_chunk1_id:\t\t\t%s\n", wh->sub_chunk1_id);
    printf ("sub_chunk1_size:\t\t%d\n", wh->sub_chunk1_size);
    printf ("audio_format:\t\t\t%d\n", wh->audio_format);
    printf ("num_channels:\t\t\t%d\n", wh->num_channels);
    printf ("sample_rate:\t\t\t%d\n", wh->sample_rate);
    printf ("bits_per_sample:\t\t%d\n", wh->bits_per_sample);
    printf ("block_align:\t\t\t%d\n", wh->block_align);
    printf ("byte_rate:\t\t\t%d\n", wh->byte_rate);
    printf ("sub_chunk2_id:\t\t\t%s\n", wh->sub_chunk2_id);
    printf ("sub_chunk2_size:\t\t%d\n", wh->sub_chunk2_size);
    printf ("=========================================\n");
}

int convertPCM2WAV(FILE *fpcm,FILE *fwav){

    int raw_sz = 0;
    wav_header_t wheader;

    memset (&wheader, '\0', sizeof (wav_header_t));


    // dummy raw pcm data
    char *pcm_buf = dummy_get_raw_pcm (fpcm, &raw_sz);

    // construct wav header
    get_wav_header (raw_sz, &wheader);
    dump_wav_header (&wheader);

    fwrite(&wheader, 1, sizeof(wheader), fwav);
    fwrite(pcm_buf, 1, raw_sz, fwav);

    if (pcm_buf)
        free (pcm_buf);


    return 0;

}