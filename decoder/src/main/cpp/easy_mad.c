#include "easy_mad.h"

#define MP3_BUF_SIZE 4096
#define MP3_FRAME_SIZE 2881

typedef struct buffer {
    FILE *fin;
    FILE *fout;
    char mp3_buf[MP3_BUF_SIZE];
    unsigned char const *start;
    unsigned long length;
} Mp3_Handle;

static
enum mad_flow input(void *data,
                    struct mad_stream *stream)
{
//    struct buffer *buffer = data;
//    if (!buffer->length)
//        return MAD_FLOW_STOP;
//    mad_stream_buffer(stream, buffer->start, buffer->length);
//    buffer->length = 0;
//    return MAD_FLOW_CONTINUE;
    Mp3_Handle *handle = (Mp3_Handle *)data;
    char *mp3_buf = handle->mp3_buf;
    /* MP3 data buffer. */
    int keep; /* Number of bytes to keep from the previous buffer. */
    int retval; /* Return value from read(). */
    int len; /* Length of the new buffer. */
    int eof; /* Whether this is the last buffer that we can provide. */

    /* Figure out how much data we need to move from the end of the previous
    buffer into the start of the new buffer. */
    if (stream->error != MAD_ERROR_BUFLEN) {
        /* All data has been consumed, or this is the first call. */
        keep = 0;
    } else if (stream->next_frame != NULL) {
        /* The previous buffer was consumed partially. Move the unconsumed portion
        into the new buffer. */
        keep = stream->bufend - stream->next_frame;
    } else if ((stream->bufend - stream->buffer) < MP3_BUF_SIZE) {
        /* No data has been consumed at all, but our read buffer isn't full yet,
        so let's just read more data first. */
        keep = stream->bufend - stream->buffer;
    } else {
        /* No data has been consumed at all, and our read buffer is already full.
        Shift the buffer to make room for more data, in such a way that any
        possible frame position in the file is completely in the buffer at least
        once. */
        keep = MP3_BUF_SIZE - MP3_FRAME_SIZE;
    }

    /* Shift the end of the previous buffer to the start of the new buffer if we
    want to keep any bytes. */
    if (keep) {
        memmove(mp3_buf, stream->bufend - keep, keep);
    }


    /* Append new data to the buffer. */
    retval = read(handle->fin, mp3_buf + keep, MP3_BUF_SIZE - keep);
    if (retval < 0) {
        /* Read error. */
        perror("failed to read from input");
        return MAD_FLOW_STOP;
    } else if (retval == 0) {
        /* End of file. Append MAD_BUFFER_GUARD zero bytes to make sure that the
        last frame is properly decoded. */
        if (keep + MAD_BUFFER_GUARD <= MP3_BUF_SIZE) {
            /* Append all guard bytes and stop decoding after this buffer. */
            memset(mp3_buf + keep, 0, MAD_BUFFER_GUARD);
            len = keep + MAD_BUFFER_GUARD;
            eof = 1;
        } else {
            /* The guard bytes don't all fit in our buffer, so we need to continue
            decoding and write all fo teh guard bytes in the next call to input(). */
            memset(mp3_buf + keep, 0, MP3_BUF_SIZE - keep);
            len = MP3_BUF_SIZE;
            eof = 0;
        }
    } else {
        /* New buffer length is amount of bytes that we kept from the previous
        buffer plus the bytes that we read just now. */
        len = keep + retval;
        eof = 0;
    }

    /* Pass the new buffer information to libmad. */
    mad_stream_buffer(stream, mp3_buf, len);
    return eof ? MAD_FLOW_STOP : MAD_FLOW_CONTINUE;
}

static inline
signed int scale(mad_fixed_t sample)
{
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));
    /* clip */
    if (sample >= MAD_F_ONE)
        sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE)
        sample = -MAD_F_ONE;
    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}

static
enum mad_flow output(void *data,
                     struct mad_header const *header,
                     struct mad_pcm *pcm)
{
    unsigned int nchannels, nsamples, rate;
    mad_fixed_t const *left_ch, *right_ch;
    rate = pcm->samplerate;
    nchannels = pcm->channels;
    nsamples  = pcm->length;
    left_ch   = pcm->samples[0];
    right_ch  = pcm->samples[1];


    int len = nsamples*2*nchannels;
    char wbuf[len];
    char * wptr = wbuf;
    while (nsamples--) {
        signed int sample;
        sample = scale(*left_ch++);
        *(wptr++) = ((sample >> 0) & 0xff);
        *(wptr++) = ((sample >> 8) & 0xff);
        if (nchannels == 2) {
            sample = scale(*right_ch++);
            *(wptr++) = ((sample >> 0) & 0xff);
            *(wptr++) = ((sample >> 8) & 0xff);
        }
    }
    Mp3_Handle * handle = (Mp3_Handle *) data;
    fwrite(wbuf,len,1,handle->fout);

    return MAD_FLOW_CONTINUE;
}

static
enum mad_flow error(void *data,
                    struct mad_stream *stream,
                    struct mad_frame *frame)
{
    struct buffer *buffer = data;
    fprintf(stderr, "decoding error 0x%04x (%s) at byte offset %u\n",
            stream->error, mad_stream_errorstr(stream),
            stream->this_frame - buffer->start);
    return MAD_FLOW_CONTINUE;
}

static
int decode(/*unsigned char const *start, unsigned long length8*/FILE *src, FILE *dest)
{
    struct buffer buffer;
    struct mad_decoder decoder;

    int result;
    buffer.fin = src;
    buffer.fout = dest;
//    buffer.start  = start;
//    buffer.length = length;
    mad_decoder_init(&decoder, &buffer,
                     input, 0 /* header */, 0 /* filter */, output,
                     error, 0 /* message */);
    result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);
    mad_decoder_finish(&decoder);
    return result;
}

int convertMP32PCM(const char *src,const char *dest){


    FILE *mp3 = open(src,O_RDONLY);
    if(mp3 == NULL){
        return -1;
    }


    FILE *pcm = fopen(dest,"wb+");
    int result = decode(mp3,pcm);


    fclose(pcm);
    close(mp3);

    return result;
}
