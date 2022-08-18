//
// Created by DZ0400351 on 2022/8/2.
//

#ifndef MY_APPLICATION_FFMPEGMEDIAPLAYER_H
#define MY_APPLICATION_FFMPEGMEDIAPLAYER_H


#include <VideoDecoder.h>
#include <AudioDecoder.h>
#include <jni.h>
#include <OpenSLRender.h>
#include <NativeRender.h>

class FFMpegMediaPlayer : public MediaPlayer {
public:
    FFMpegMediaPlayer() {};

    virtual ~FFMpegMediaPlayer() {

    };

    virtual void
    Init(JNIEnv *jniEnv, jobject obj, const char *url, int renderType, jobject surface);

    virtual void UnInit();

    virtual void Play();

    virtual void Pause();

    virtual void Stop();

    virtual void SeekToPosition(float position);

    virtual long GetMediaParams(int paramType);


    void onProcess(float process);

    void requestRender();

private:
    virtual JNIEnv *GetJNIEnv(bool *isAttach);

    virtual jobject GetJavaObj();

    virtual JavaVM *GetJavaVM();

    VideoDecoder *mVideoDecoder = nullptr;
    AudioDecoder *mAudioDecoder = nullptr;
    VideoRender *mVideoRender = nullptr;
    AudioRender *mAudioRender = nullptr;


    jobject mMediaPlayerCallBackObj = nullptr;
    jmethodID mMediaPlayerCallBackOnPlayProcess = nullptr;
    jmethodID mMediaPlayerCallBackRequestRenderProcess = nullptr;
};


#endif //MY_APPLICATION_FFMPEGMEDIAPLAYER_H
