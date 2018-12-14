//
// Created by fulongbin on 2018/12/12.
//

#ifndef SILK_V3_DECODER_EASY_MAD_H
#define SILK_V3_DECODER_EASY_MAD_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <memory.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <sys/ioctl.h>
# include <sys/fcntl.h>
# include <sys/types.h>
#include "libmad/mad.h"



int convertMP32PCM(const char *src,const char *dest);

#endif //SILK_V3_DECODER_EASY_MAD_H
