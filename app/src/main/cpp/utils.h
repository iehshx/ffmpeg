//
// Created by DZ0400351 on 2022/7/29.
//

#ifndef MY_APPLICATION_UTILS_H
#define MY_APPLICATION_UTILS_H

#define DELAY_THRESHOLD 100

static long long GetSysCurrentTime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    long long curTime = ((long long) (time.tv_sec)) * 1000 + time.tv_usec / 1000;
    return curTime;
}

#endif //MY_APPLICATION_UTILS_H
