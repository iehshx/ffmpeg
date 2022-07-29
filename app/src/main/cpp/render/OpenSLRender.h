//
// Created by DZ0400351 on 2022/7/28.
//

#ifndef MY_APPLICATION_OPENSLRENDER_H
#define MY_APPLICATION_OPENSLRENDER_H
#define MAX_QUEUE_BUFFER_SIZE 3

#include <jni.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <sys/types.h>
#include <stdlib.h>
#include <cstdint>
#include <queue>
#include <string>
#include <thread>

extern "C" {
#include <libavutil/rational.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include "../MLOG.h"
#include "AudioRender.h"

using namespace std;

class OpenSLRender : public AudioRender {

public:
    OpenSLRender() {}

    virtual ~OpenSLRender() {}

    virtual void Init();

    virtual void ClearAudioCache();

    virtual void RenderAudioFrame(uint8_t *pData, int dataSize);

    virtual void UnInit();

private:
    /**
     * opensles
     */
    SLObjectItf mEngineObj = nullptr;
    SLEngineItf mEngineEngine = nullptr;//播放引擎
    SLObjectItf mOutputMixObj = nullptr;//混音器
//    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = nullptr;
//    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    //播放器相关资源
    SLObjectItf mAudioPlayerObj = nullptr;
    SLPlayItf mAudioPlayerPlay;
    SLmilliHertz bqPlayerSampleRate = 0;
    SLAndroidSimpleBufferQueueItf mBufferQueue;
    SLVolumeItf mAudioPlayerVolume;
    SLEffectSendItf bqPlayerEffectSend;

    bool createEngine();
    bool CreateOutputMixer();

    bool createPlayer();

    int GetAudioFrameQueueSize();

    void StartRender();

    void HandleAudioFrameQueue();

    static void CreateSLWaitingThread(OpenSLRender *openSlRender);

    static void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context);

    queue<AudioFrame *> mAudioFrameQueue;
    thread *mThread = nullptr;
    mutex mMutex;
    condition_variable mCond;
    volatile bool mExit = false;

};


#endif //MY_APPLICATION_OPENSLRENDER_H
