//
// Created by DZ0400351 on 2022/7/29.
//

#ifndef MY_APPLICATION_UTILS_H
#define MY_APPLICATION_UTILS_H

#define DELAY_THRESHOLD 100
#include "stdlib.h"
#include "time.h"

static long long GetSysCurrentTime() {
    struct timeval time;
    gettimeofday(&time, nullptr);
    long long curTime = ((long long) (time.tv_sec)) * 1000 + time.tv_usec / 1000;
    return curTime;
}

typedef struct _tag_NativeImage
{
    int width;
    int height;
    int format;
    uint8_t *ppPlane[3];
    int pLineSize[3];
    _tag_NativeImage()
    {
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

#endif //MY_APPLICATION_UTILS_H
