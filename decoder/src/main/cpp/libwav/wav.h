//
// Created by fulongbin on 2019/2/1.
//

#ifndef SILK_V3_DECODER_WAV_H
#define SILK_V3_DECODER_WAV_H
#include <android/log.h>
#include <stdio.h>

int convertWAV2PCM(FILE *fwav,FILE *fpcm);
int convertPCM2WAV(FILE *fpcm,FILE *fwav);


#endif //SILK_V3_DECODER_WAV_H
