//
// Created by DZ0400351 on 2022/7/28.
//

#ifndef MY_APPLICATION_AUDIOPLAYER_H
#define MY_APPLICATION_AUDIOPLAYER_H

#include <jni.h>
#include <AudioDecoder.h>
#include <AudioRender.h>
#include "MediaPlayer.h"

class AudioPlayer : public MediaPlayer {

public:
    AudioPlayer() {};

    virtual ~AudioPlayer() {};

    virtual void Init(JNIEnv *jniEnv, jobject obj, const char *url, int renderType, jobject surface);

    virtual void UnInit();

    virtual void Play();

    virtual void Pause();

    virtual void Stop();

    virtual void SeekToPosition(float position);

    virtual long GetMediaParams(int paramType);

private :
    AudioDecoder *mAudioDecoder = nullptr;
    AudioRender *mAudioRender = nullptr;

    virtual JavaVM *GetJavaVM();

    virtual jobject GetJavaObj();

    virtual JNIEnv *GetJNIEnv(bool *isAttach);

    //    /**
//     * jni回调
//     */
    jobject audioPlayerCallBackObj = nullptr;
    jmethodID audioPlayerCallBackOnError = nullptr;
    jmethodID audioPlayerCallBackOnPrepareSuccess = nullptr;
    jmethodID audioPlayerCallBackOnPlay = nullptr;
};


#endif //MY_APPLICATION_AUDIOPLAYER_H