//
// Created by DZ0400351 on 2022/7/28.
//

#ifndef MY_APPLICATION_MEDIAPLAYER_H
#define MY_APPLICATION_MEDIAPLAYER_H

class MediaPlayer {
public:
    MediaPlayer() {};

    virtual ~MediaPlayer() {};

    virtual void
    Init(JNIEnv *jniEnv, jobject obj, const char *url, int renderType, jobject surface) = 0;

    virtual void UnInit() = 0;

    virtual void Play() = 0;

    virtual void Pause() = 0;

    virtual void Stop() = 0;

    virtual void SeekToPosition(float position) = 0;

    virtual long GetMediaParams(int paramType) = 0;

    virtual void SetMediaParams(int paramType, jobject obj) {}

    virtual JNIEnv *GetJNIEnv(bool *isAttach) = 0;

    virtual jobject GetJavaObj() = 0;

    virtual JavaVM *GetJavaVM() = 0;

    JavaVM *mJavaVM = nullptr;
    jobject mJavaObj = nullptr;
};

#endif //MY_APPLICATION_MEDIAPLAYER_H
