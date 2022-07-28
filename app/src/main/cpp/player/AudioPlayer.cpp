//
// Created by DZ0400351 on 2022/7/28.
//

#include <OpenSLRender.h>
#include "AudioPlayer.h"

void
AudioPlayer::Init(JNIEnv *jniEnv, jobject obj, const char *url, int renderType, jobject surface) {
    jniEnv->GetJavaVM(&mJavaVM);
    mJavaObj = jniEnv->NewGlobalRef(obj);
    mAudioDecoder = new AudioDecoder(url);
    mAudioRender = new OpenSLRender();
    mAudioDecoder->SetAudioRender(mAudioRender);
}

void AudioPlayer::UnInit() {
    if (mAudioDecoder) {
        delete mAudioDecoder;
        mAudioDecoder = nullptr;
    }
    if (mAudioRender) {
        delete mAudioRender;
        mAudioRender = nullptr;
    }

    if (audioPlayerCallBackObj) {
        bool isAttach = false;
        JNIEnv *env = GetJNIEnv(&isAttach);
        env->DeleteGlobalRef(audioPlayerCallBackObj);
        audioPlayerCallBackObj = nullptr;
        audioPlayerCallBackOnPrepareSuccess = nullptr;
        audioPlayerCallBackOnError = nullptr;
    }

    bool isAttach = false;
    GetJNIEnv(&isAttach)->DeleteGlobalRef(mJavaObj);
    if (isAttach)
        GetJavaVM()->DetachCurrentThread();
}

void AudioPlayer::Play() {
    if (mAudioDecoder)
        mAudioDecoder->Start();
}

void AudioPlayer::Pause() {

    if (mAudioDecoder)
        mAudioDecoder->Pause();
}

void AudioPlayer::Stop() {
    if (mAudioDecoder)
        mAudioDecoder->Stop();
}

void AudioPlayer::SeekToPosition(float position) {
    if (mAudioDecoder)
        mAudioDecoder->SeekToPosition(position);
}

long AudioPlayer::GetMediaParams(int paramType) {
    return 0;
}

JavaVM *AudioPlayer::GetJavaVM() {
    return mJavaVM;
}

JNIEnv *AudioPlayer::GetJNIEnv(bool *isAttach) {
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

jobject AudioPlayer::GetJavaObj() {
    return mJavaObj;
}
