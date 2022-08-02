//
// Created by DZ0400351 on 2022/8/2.
//

#include "FFMpegMediaPlayer.h"

void
FFMpegMediaPlayer::Init(JNIEnv *jniEnv, jobject obj, const char *url, int renderType,
                        jobject surface) {
    jniEnv->GetJavaVM(&mJavaVM);
    mJavaObj = jniEnv->NewGlobalRef(obj);
    mVideoDecoder = new VideoDecoder(url);
    mVideoRender = new NativeRender(jniEnv, surface);
    mVideoDecoder->SetVideoRender(mVideoRender);
    mAudioDecoder = new AudioDecoder(url);
    mAudioRender = new OpenSLRender();
    mAudioDecoder->SetAudioRender(mAudioRender);
    mAudioDecoder->SetMessageCallback(this);


    jclass audioPlayerCallBack = jniEnv->FindClass(
            "com/example/myapplication/MediaPlayer$MediaPlayerCallBack");
    jclass audioPlayer = jniEnv->FindClass(
            "com/example/myapplication/MediaPlayer");
    jfieldID callBackFieldId = jniEnv->GetFieldID(audioPlayer, "callBack",
                                                  "Lcom/example/myapplication/MediaPlayer$MediaPlayerCallBack;");
    mMediaPlayerCallBackObj = jniEnv->NewGlobalRef(
            jniEnv->GetObjectField(obj, callBackFieldId));
    if (!mMediaPlayerCallBackOnPlayProcess) {
        mMediaPlayerCallBackOnPlayProcess = jniEnv->GetMethodID(audioPlayerCallBack, "onPlayProcess",
                                                          "(F)V");
    }
}

void FFMpegMediaPlayer::UnInit() {

    if (mVideoRender) {
        delete mVideoRender;
        mVideoRender = nullptr;
    }

    if (mVideoDecoder) {
        delete mVideoDecoder;
        mVideoDecoder = nullptr;
    }

    if (mAudioDecoder) {
        delete mAudioDecoder;
        mAudioDecoder = nullptr;
    }
    if (mAudioRender) {
        delete mAudioRender;
        mAudioRender = nullptr;
    }

    if (mMediaPlayerCallBackObj) {
        bool isAttach = false;
        JNIEnv *env = GetJNIEnv(&isAttach);
        env->DeleteGlobalRef(mMediaPlayerCallBackObj);
        mMediaPlayerCallBackOnPlayProcess = nullptr;
    }

    bool isAttach = false;
    GetJNIEnv(&isAttach)->DeleteGlobalRef(mJavaObj);
    if (isAttach)
        GetJavaVM()->DetachCurrentThread();
}

void FFMpegMediaPlayer::Play() {

    if (mAudioDecoder)
        mAudioDecoder->Start();

    if (mVideoDecoder)
        mVideoDecoder->Start();
}

void FFMpegMediaPlayer::Pause() {

    if (mAudioDecoder)
        mAudioDecoder->Pause();
    if (mVideoDecoder)
        mVideoDecoder->Pause();
}

void FFMpegMediaPlayer::Stop() {

    if (mAudioDecoder)
        mAudioDecoder->Stop();

    if (mVideoDecoder)
        mVideoDecoder->Stop();

}

void FFMpegMediaPlayer::SeekToPosition(float position) {

}

long FFMpegMediaPlayer::GetMediaParams(int paramType) {
    if (mAudioDecoder)
        return mAudioDecoder->GetDuration();
    return 0;
}

JNIEnv *FFMpegMediaPlayer::GetJNIEnv(bool *isAttach) {
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

jobject FFMpegMediaPlayer::GetJavaObj() {
    return mJavaObj;
}

JavaVM *FFMpegMediaPlayer::GetJavaVM() {
    return mJavaVM;
}

void FFMpegMediaPlayer::onProcess(float process) {
    if (mMediaPlayerCallBackObj) {
        bool isAttach = false;
        JNIEnv *env = GetJNIEnv(&isAttach);
        env->CallVoidMethod(mMediaPlayerCallBackObj, mMediaPlayerCallBackOnPlayProcess, process);
        GetJavaVM()->DetachCurrentThread();
    }
}
