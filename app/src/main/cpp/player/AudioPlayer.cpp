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
    mAudioDecoder->SetMessageCallback(this);
    jclass audioPlayerCallBack = jniEnv->FindClass(
            "com/example/myapplication/AudioPlayer$AudioPlayerCallBack");
    jclass audioPlayer = jniEnv->FindClass(
            "com/example/myapplication/AudioPlayer");
    jfieldID callBackFieldId = jniEnv->GetFieldID(audioPlayer, "callback",
                                                  "Lcom/example/myapplication/AudioPlayer$AudioPlayerCallBack;");
    mAudioPlayerCallBackObj = jniEnv->NewGlobalRef(
            jniEnv->GetObjectField(obj, callBackFieldId));
    if (!mAudioPlayerCallBackOnError) {
        mAudioPlayerCallBackOnError = jniEnv->GetMethodID(audioPlayerCallBack, "onError",
                                                          "(I)V");
    }
    if (!mAudioPlayerCallBackOnPrepare) {
        mAudioPlayerCallBackOnPrepare = jniEnv->GetMethodID(audioPlayerCallBack,
                                                            "onPrepareSuccess",
                                                            "()V");
    }
    if (!mAudioPlayerCallBackOnPlayProcess) {
        mAudioPlayerCallBackOnPlayProcess = jniEnv->GetMethodID(audioPlayerCallBack,
                                                                "onPlayProcess",
                                                                "(F)V");
    }


    onPrepareProcess();


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

    if (mAudioPlayerCallBackObj) {
        bool isAttach = false;
        JNIEnv *env = GetJNIEnv(&isAttach);
        env->DeleteGlobalRef(mAudioPlayerCallBackObj);
        mAudioPlayerCallBackObj = nullptr;
        mAudioPlayerCallBackOnPrepare = nullptr;
        mAudioPlayerCallBackOnError = nullptr;
        mAudioPlayerCallBackOnPlayProcess = nullptr;
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
    if (mAudioDecoder) {
        mAudioDecoder->Stop();
    }
}

void AudioPlayer::SeekToPosition(float position) {
    if (mAudioDecoder)
        mAudioDecoder->SeekToPosition(position);
}

long AudioPlayer::GetMediaParams(int paramType) {
    if (paramType == 0) {
        return mAudioDecoder->GetDuration();
    } else {
        return 0;
    }
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


void AudioPlayer::onPrepareProcess() {
    if (mAudioPlayerCallBackOnPrepare) {
        bool isAttach = false;
        JNIEnv *env = GetJNIEnv(&isAttach);
        env->CallVoidMethod(mAudioPlayerCallBackObj, mAudioPlayerCallBackOnPrepare);
    }
}

void AudioPlayer::onProcess(float process) {
    if (mAudioPlayerCallBackOnPlayProcess) {
        bool isAttach = false;
        JNIEnv *env = GetJNIEnv(&isAttach);
        env->CallVoidMethod(mAudioPlayerCallBackObj, mAudioPlayerCallBackOnPlayProcess,
                            process);
        if (isAttach) {
            mJavaVM->DetachCurrentThread();
        }
    }
}

AudioPlayer::~AudioPlayer() {

    if (mAudioDecoder) {
        mAudioDecoder->Stop();
        delete mAudioDecoder;
        mAudioDecoder = nullptr;
    }
    if (mAudioRender) {
        mAudioRender->UnInit();
        delete mAudioRender;
        mAudioRender = nullptr;
    }
    if (mAudioPlayerCallBackObj) {
        delete mAudioPlayerCallBackObj;
        mAudioPlayerCallBackObj = nullptr;
    }
    if (mAudioPlayerCallBackOnPrepare) {
        delete mAudioPlayerCallBackOnPrepare;
        mAudioPlayerCallBackOnPrepare = nullptr;
    }
    if (mAudioPlayerCallBackOnError) {
        delete mAudioPlayerCallBackOnError;
        mAudioPlayerCallBackOnError = nullptr;
    }

    if (mAudioPlayerCallBackOnPlayProcess) {
        delete mAudioPlayerCallBackOnPlayProcess;
        mAudioPlayerCallBackOnPlayProcess = nullptr;
    }
}

void AudioPlayer::requestRender() {

}

