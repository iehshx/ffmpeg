#include <jni.h>
#include <FFMpegMediaPlayer.h>
#include "GLRender.h"

//
// Created by DZ0400351 on 2022/7/28.
//


extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_myapplication_MediaPlayer_nativeInit(JNIEnv *env, jobject thiz) {
    FFMpegMediaPlayer *ffMpegMediaPlayer = new FFMpegMediaPlayer();
    return reinterpret_cast<jlong>(ffMpegMediaPlayer);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MediaPlayer_nStop(JNIEnv *env, jobject thiz, jlong ptr) {
    FFMpegMediaPlayer *ffMpegMediaPlayer = reinterpret_cast<FFMpegMediaPlayer *>(ptr);
    if (ffMpegMediaPlayer) {
        ffMpegMediaPlayer->Stop();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MediaPlayer_nSeek(JNIEnv *env, jobject thiz, jlong ptr) {
    FFMpegMediaPlayer *ffMpegMediaPlayer = reinterpret_cast<FFMpegMediaPlayer *>(ptr);
    if (ffMpegMediaPlayer) {
//        audioPlayer->SeekToPosition();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MediaPlayer_nPause(JNIEnv *env, jobject thiz, jlong ptr) {
    FFMpegMediaPlayer *ffMpegMediaPlayer = reinterpret_cast<FFMpegMediaPlayer *>(ptr);
    if (ffMpegMediaPlayer) {
        ffMpegMediaPlayer->Pause();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MediaPlayer_nPlay(JNIEnv *env, jobject thiz, jlong ptr) {
    FFMpegMediaPlayer *ffMpegMediaPlayer = reinterpret_cast<FFMpegMediaPlayer *>(ptr);
    if (ffMpegMediaPlayer) {
        ffMpegMediaPlayer->Play();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MediaPlayer_nPrepare(JNIEnv *env, jobject thiz, jlong ptr,
                                                    jstring jUrl, jint renderType,
                                                    jobject surface) {
    FFMpegMediaPlayer *ffMpegMediaPlayer = reinterpret_cast<FFMpegMediaPlayer *>(ptr);
    if (ffMpegMediaPlayer) {
        const char *url = env->GetStringUTFChars(jUrl, JNI_FALSE);
        ffMpegMediaPlayer->Init(env, thiz, url, renderType, surface);
        env->ReleaseStringUTFChars(jUrl, url);
    }
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_myapplication_MediaPlayer_nGetDuration(JNIEnv *env, jobject thiz, jlong ptr) {
    FFMpegMediaPlayer *ffMpegMediaPlayer = reinterpret_cast<FFMpegMediaPlayer *>(ptr);
    if (ffMpegMediaPlayer) {
        return ffMpegMediaPlayer->GetMediaParams(0);
    }
    return 0L;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MediaPlayer_nUnInit(JNIEnv *env, jobject thiz, jlong ptr) {
    FFMpegMediaPlayer *ffMpegMediaPlayer = reinterpret_cast<FFMpegMediaPlayer *>(ptr);
    if (ffMpegMediaPlayer) {
        ffMpegMediaPlayer->UnInit();
    }
}

/**
 * 设置渲染模式
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MediaPlayer_00024Companion_onSurfaceCreated(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jint renderType, jint renderStyle) {

    switch (renderType) {
        case VIDEO_RENDER_OPENGL:
            GLRender::GetInstance()->OnSurfaceCreated(renderStyle);
            break;
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MediaPlayer_00024Companion_onDrawFrame(JNIEnv *env, jobject thiz,
                                                                      jint render_type) {
    GLRender::GetInstance()->OnDrawFrame();
}