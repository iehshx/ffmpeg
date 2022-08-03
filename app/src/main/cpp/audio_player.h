////
//// Created by DZ0400351 on 2022/7/27.
////
//
//#ifndef MY_APPLICATION_AUDIO_H
//
//#include <jni.h>
//
//#include <SLES/OpenSLES.h>
//#include <SLES/OpenSLES_Android.h>
//#include <sys/types.h>
//#include <stdlib.h>
//#include <pthread.h>
//
//extern "C" {
//#include <libavutil/rational.h>
//#include <libavformat/avformat.h>
//#include <libswresample/swresample.h>
//};
//
//#include "MLOG.h"
//
//#define STATE_ERROR_CODE_INIT_ENGINE  1
//#define STATE_ERROR_MSG_INIT_ENGINE  "创建引擎失败"
//#define STATE_ERROR_CODE_INIT_PLAYER  2
//#define STATE_ERROR_MSG_INIT_PLAYER "创建播放器失败"
//#define STATE_CODE_PLAY_SUCCESS  3
//#define STATE_CODE_PLAY_ERROR  4
//#define MY_APPLICATION_AUDIO_H
//
//enum THREAD_STATE {
//    MAIN, CHILD
//};
//
//class AudioPlayer {
//public:
//
//    AudioPlayer(JNIEnv *jniEnv, jobject pJobject);
//
//    JavaVM *vm = NULL;
//
//    int current = 0;
//    int lastTime = 0;
//    int total = 0;
//
///**
//     * 创建sl 引擎
//     */
//    void init(THREAD_STATE state);
//
//    void setAudioParams(int sampleRate, int bufSize);
//
//    /**
//     * 开始播放
//     */
//    void play();
//
//    SLVolumeItf getVolume();
//
//    void callOnStateChange(JNIEnv *env, int code);
//
//    void callOnPrepareSuccess(JNIEnv *env);
//
//    void callOnPlay(JNIEnv *env, int current, int total);
//
//    ~AudioPlayer();
//
//
//    void equeue(uint8_t *string);
//
//    void stop();
//
//    AVRational time_base;
//    int sampleRate = 0;
//    int bufSize = 0;
//private:
//
//
//    /**
//     * ffmpeg
//     */
//    AVFormatContext *avFormatContext = nullptr;
//    AVCodecContext *avCodecContext = nullptr;
//    SwrContext *swrContext = swr_alloc();
//    /**
//     * opensles
//     */
//    SLObjectItf mEngineObj = nullptr;
//    SLEngineItf mEngineEngine = nullptr;//播放引擎
//    SLObjectItf mOutputMixObj = nullptr;//混音器
//    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = nullptr;
//    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
//    //播放器相关资源
//    SLObjectItf mAudioPlayerObj = nullptr;
//    SLPlayItf mAudioPlayerPlay;
//    SLmilliHertz bqPlayerSampleRate = 0;
//    SLAndroidSimpleBufferQueueItf mBufferQueue;
//    SLVolumeItf mAudioPlayerVolume;
//    SLEffectSendItf bqPlayerEffectSend;
//
//    /**
//     * jni回调
//     */
//    jobject mAudioPlayerCallBackObj = nullptr;
//    jmethodID mAudioPlayerCallBackOnError = nullptr;
//    jmethodID audioPlayerCallBackOnPrepareSuccess = nullptr;
//    jmethodID mAudioPlayerCallBackOnPlayProcess = nullptr;
//
//
//    bool createEngine();
//
//    bool createPlayer();
//
//    bool createBufferQueueAudioPlayer();
//
//    void release();
//
//    bool checkSlresult(SLresult sLresult);
//
//
//    JNIEnv *jniEnv = nullptr;
//
//
//    bool createBufferQueueAudioDecoder();
//};
//
//
//#endif //MY_APPLICATION_AUDIO_H
