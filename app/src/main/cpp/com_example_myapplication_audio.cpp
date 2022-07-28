#include <jni.h>
#include <AudioPlayer.h>

//
// Created by DZ0400351 on 2022/7/28.
//


extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_myapplication_AudioPlayer_nativeInit(JNIEnv *env, jobject thiz, jstring jUrl) {
    AudioPlayer *audioPlayer = new AudioPlayer();
    const char *url = env->GetStringUTFChars(jUrl, JNI_FALSE);
    audioPlayer->Init(env, thiz, url, 0, nullptr);
    return reinterpret_cast<jlong>(audioPlayer);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_AudioPlayer_nStop(JNIEnv *env, jobject thiz, jlong ptr) {
    AudioPlayer *audioPlayer = reinterpret_cast<AudioPlayer *>(ptr);
    if (audioPlayer) {
        audioPlayer->Stop();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_AudioPlayer_nSeek(JNIEnv *env, jobject thiz, jlong ptr) {
    AudioPlayer *audioPlayer = reinterpret_cast<AudioPlayer *>(ptr);
    if (audioPlayer) {
//        audioPlayer->SeekToPosition();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_AudioPlayer_nPause(JNIEnv *env, jobject thiz, jlong ptr) {
    AudioPlayer *audioPlayer = reinterpret_cast<AudioPlayer *>(ptr);
    if (audioPlayer) {
        audioPlayer->Pause();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_AudioPlayer_nPlay(JNIEnv *env, jobject thiz, jlong ptr) {
    AudioPlayer *audioPlayer = reinterpret_cast<AudioPlayer *>(ptr);
    if (audioPlayer) {
        audioPlayer->Play();
    }
}