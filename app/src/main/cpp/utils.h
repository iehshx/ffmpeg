//
// Created by DZ0400351 on 2022/7/29.
//

#ifndef MY_APPLICATION_UTILS_H
#define MY_APPLICATION_UTILS_H

#define DELAY_THRESHOLD 100


#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "stdio.h"
#include "sys/stat.h"
#include "stdint.h"
#include "time.h"
#include "MLOG.h"

#define IMAGE_FORMAT_RGBA           0x01
#define IMAGE_FORMAT_RGBA_EXT       "RGB32"

static long long GetSysCurrentTime() {
    struct timeval time;
    gettimeofday(&time, nullptr);
    long long curTime = ((long long) (time.tv_sec)) * 1000 + time.tv_usec / 1000;
    return curTime;
}

typedef struct _tag_NativeImage {
    int width;
    int height;
    int format;
    uint8_t *ppPlane[3];
    int pLineSize[3];

    _tag_NativeImage() {
        width = 0;
        height = 0;
        format = 0;
        ppPlane[0] = nullptr;
        ppPlane[1] = nullptr;
        ppPlane[2] = nullptr;
        pLineSize[0] = 0;
        pLineSize[1] = 0;
        pLineSize[2] = 0;
    }
} NativeImage;


class NativeImageUtil {
public:

    static void AllocNativeImage(NativeImage *pImage) {
        if (pImage->height == 0 || pImage->width == 0) return;
        pImage->ppPlane[0] = static_cast<uint8_t *>(malloc(pImage->width * pImage->height * 4));
        pImage->pLineSize[0] = pImage->width * 4;
        pImage->pLineSize[1] = 0;
        pImage->pLineSize[2] = 0;
    }

    static void CopyNativeImage(NativeImage *pSrcImg, NativeImage *pDstImg) {

        LOGE("NativeImageUtil::CopyNativeImage src[w,h,format]=[%d, %d, %d], dst[w,h,format]=[%d, %d, %d]", pSrcImg->width, pSrcImg->height, pSrcImg->format, pDstImg->width, pDstImg->height, pDstImg->format);
        LOGE("NativeImageUtil::CopyNativeImage src[line0,line1,line2]=[%d, %d, %d], dst[line0,line1,line2]=[%d, %d, %d]", pSrcImg->pLineSize[0], pSrcImg->pLineSize[1], pSrcImg->pLineSize[2], pDstImg->pLineSize[0], pDstImg->pLineSize[1], pDstImg->pLineSize[2]);
        if(pSrcImg == nullptr || pSrcImg->ppPlane[0] == nullptr) return;

        if(pSrcImg->format != pDstImg->format ||
           pSrcImg->width != pDstImg->width ||
           pSrcImg->height != pDstImg->height)
        {
            LOGE("NativeImageUtil::CopyNativeImage invalid params.");
            return;
        }

        if(pDstImg->ppPlane[0] == nullptr) AllocNativeImage(pDstImg);

        if(pSrcImg->pLineSize[0] != pDstImg->pLineSize[0])
        {
            for (int i = 0; i < pSrcImg->height; ++i) {
                memcpy(pDstImg->ppPlane[0] + i * pDstImg->pLineSize[0], pSrcImg->ppPlane[0] + i * pSrcImg->pLineSize[0], pDstImg->width * 4);
            }
        } else {
            memcpy(pDstImg->ppPlane[0], pSrcImg->ppPlane[0], pSrcImg->pLineSize[0] * pSrcImg->height);
        }

    }

    static void FreeNativeImage(NativeImage *pImage) {
        if (pImage == nullptr || pImage->ppPlane[0] == nullptr) return;
        free(pImage->ppPlane[0]);
        pImage->ppPlane[0] = nullptr;
        pImage->ppPlane[1] = nullptr;
        pImage->ppPlane[2] = nullptr;
    }
};

#endif //MY_APPLICATION_UTILS_H
