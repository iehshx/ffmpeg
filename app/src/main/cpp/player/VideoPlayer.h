//
// Created by DZ0400351 on 2022/8/2.
//

#ifndef MY_APPLICATION_VIDEOPLAYER_H
#define MY_APPLICATION_VIDEOPLAYER_H

#include <VideoDecoder.h>
#include <VideoRender.h>
#include "MediaPlayer.h"
#include "NativeRender.h"

class VideoPlayer : MediaPlayer {


public:
    VideoPlayer() {};

    virtual ~VideoPlayer();

    virtual void
    Init(JNIEnv *jniEnv, jobject obj, const char *url, int renderType, jobject surface);

    virtual void UnInit();

    virtual void Play();

    virtual void Pause();

    virtual void Stop();

    virtual void SeekToPosition(float position);

    virtual long GetMediaParams(int paramType);

    virtual void onProcess(float process);

private :

    VideoDecoder *mVideoDecoder = nullptr;
    VideoRender *mVideoRender = nullptr;


    virtual JavaVM *GetJavaVM();

    virtual jobject GetJavaObj();

    virtual JNIEnv *GetJNIEnv(bool *isAttach);

};


#endif //MY_APPLICATION_VIDEOPLAYER_H
