//
// Created by DZ0400351 on 2022/8/2.
//

#include "VideoPlayer.h"
#include "android/native_window.h"
#include "../MLOG.h"

VideoPlayer::~VideoPlayer() {
}

void
VideoPlayer::Init(JNIEnv *jniEnv, jobject obj, const char *url, int renderType, jobject surface) {
    jniEnv->GetJavaVM(&this->mJavaVM);
    mJavaObj = jniEnv->NewGlobalRef(obj);
    mVideoDecoder = new VideoDecoder(url);
    mVideoRender = new NativeRender(jniEnv, surface);
    mVideoDecoder->SetVideoRender(mVideoRender);
}

void VideoPlayer::UnInit() {

    if (mVideoRender) {
        delete mVideoRender;
        mVideoRender = nullptr;
    }

    if (mVideoDecoder) {
        delete mVideoDecoder;
        mVideoDecoder = nullptr;
    }

    bool isAttach = false;
    GetJNIEnv(&isAttach)->DeleteGlobalRef(mJavaObj);
    if (isAttach)
        GetJavaVM()->DetachCurrentThread();
}

void VideoPlayer::Play() {
    if (mVideoDecoder)
        mVideoDecoder->Start();
}

void VideoPlayer::Pause() {
    if (mVideoDecoder) {
        mVideoDecoder->Pause();
    }
}

void VideoPlayer::Stop() {
    if (mVideoDecoder)
        mVideoDecoder->Stop();
}

void VideoPlayer::SeekToPosition(float position) {

}

long VideoPlayer::GetMediaParams(int paramType) {
    return 0;
}

void VideoPlayer::onProcess(float process) {

}

JavaVM *VideoPlayer::GetJavaVM() {
    return mJavaVM;
}

jobject VideoPlayer::GetJavaObj() {
    return mJavaObj;
}

JNIEnv *VideoPlayer::GetJNIEnv(bool *isAttach) {
    JNIEnv *env;
    int status;
    if (nullptr == mJavaVM) {
        LOGE("AudioPlayer::GetJNIEnv mJavaVM == nullptr");
        return nullptr;
    }
    *isAttach = false;
    status = mJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);
    if (status != JNI_OK) {
        status = mJavaVM->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            LOGE("AudioPlayer::GetJNIEnv failed to attach current thread");
            return nullptr;
        }
        *isAttach = true;
    }
    return env;
}
