//
// Created by DZ0400351 on 2022/7/28.
//

#include "OpenSLRender.h"

using namespace std;

void OpenSLRender::Init() {
    int result = -1;
    do {
        result = createEngine();
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::Init CreateEngine fail. result=%d", result);
            break;
        }

        result = createPlayer();
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::Init CreateAudioPlayer fail. result=%d", result);
            break;
        }

        mThread = new std::thread(CreateSLWaitingThread, this);

    } while (false);

    if (result != SL_RESULT_SUCCESS) {
        LOGE("OpenSLRender::Init fail. result=%d", result);
        UnInit();
    }
}

void OpenSLRender::ClearAudioCache() {
    std::unique_lock<std::mutex> lock(mMutex);
    for (int i = 0; i < mAudioFrameQueue.size(); ++i) {
        AudioFrame *audioFrame = mAudioFrameQueue.front();
        mAudioFrameQueue.pop();
        delete audioFrame;
    }
}

void OpenSLRender::RenderAudioFrame(uint8_t *pData, int dataSize) {
    if (bqPlayerPlay) {
        if (pData != nullptr && dataSize > 0) {

            //temp resolution, when queue size is too big.
            while (GetAudioFrameQueueSize() >= MAX_QUEUE_BUFFER_SIZE && !mExit) {
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
            }

            std::unique_lock<std::mutex> lock(mMutex);
            AudioFrame *audioFrame = new AudioFrame(pData, dataSize);
            mAudioFrameQueue.push(audioFrame);
            mCond.notify_all();
            lock.unlock();
        }
    }

}

void OpenSLRender::UnInit() {

    std::unique_lock<std::mutex> lock(mMutex);
    mExit = true;
    mCond.notify_all();
    lock.unlock();


    if (outputMixObject) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixEnvironmentalReverb = nullptr;
        outputMixObject = nullptr;
        bqPlayerBufferQueue = nullptr;
        bqPlayerVolume = nullptr;
        bqPlayerEffectSend = nullptr;
    }

    if (engineObject) {
        (*engineObject)->Destroy(engineObject);
        engineObject = nullptr;
        engineEngine = nullptr;
    }
    if (bqPlayerObject) {
        (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
        bqPlayerObject = nullptr;
        bqPlayerPlay = nullptr;
    }

    lock.lock();
    for (int i = 0; i < mAudioFrameQueue.size(); ++i) {
        AudioFrame *audioFrame = mAudioFrameQueue.front();
        mAudioFrameQueue.pop();
        delete audioFrame;
    }
    lock.unlock();

    if (mThread != nullptr) {
        mThread->join();
        delete mThread;
        mThread = nullptr;
    }
//    AudioGLRender::ReleaseInstance();
}

bool OpenSLRender::createEngine() {
    SLresult result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (checkSlresult(result)) {
        result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
        if (checkSlresult(result)) {
            result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
                                                   &engineEngine);
            if (checkSlresult(result)) {
                const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
                const SLboolean req[1] = {SL_BOOLEAN_FALSE};
                result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids,
                                                          req);
                if (checkSlresult(result)) {
                    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
                    if (checkSlresult(result)) {
                        result = (*outputMixObject)->GetInterface(outputMixObject,
                                                                  SL_IID_ENVIRONMENTALREVERB,
                                                                  &outputMixEnvironmentalReverb);
                        if (checkSlresult(result)) {
                            result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                                    outputMixEnvironmentalReverb, &reverbSettings);
                            LOGE("create AudioPlayer engine success!");
                            return SL_RESULT_SUCCESS;
                        }
                    }
                }
            }
        }
    } else {
        LOGE("create AudioPlayer engine faild!");
    }
    return result;
}

bool OpenSLRender::createPlayer() {

    //支持的资源
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, (SLuint32) 2, SL_SAMPLINGRATE_44_1,
                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                                   SL_BYTEORDER_LITTLEENDIAN};

    // 声音输出设备
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ };
    SLresult result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc,
                                                         &audioSnk,
                                                         3, ids, req);
    if (checkSlresult(result)) {
        result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
        if (checkSlresult(result)) {
            result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
            if (checkSlresult(result)) {
                LOGE("create AudioPlayer player success!");
            }
        }
    } else {
        LOGE("create AudioPlayer player faild!");
    }


    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
    if (checkSlresult(result)) {
        result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, AudioPlayerCallback,
                                                          this);
        if (checkSlresult(result)) {
            // get the effect send interface
            bqPlayerEffectSend = nullptr;
            if (0 == bqPlayerSampleRate) {
                result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
                                                         &bqPlayerEffectSend);
                if (checkSlresult(result)) {
                    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME,
                                                             &bqPlayerVolume);
                    if (checkSlresult(result)) {
                        // 设置播放器播放状态
                        result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
                        LOGE("create queue and playState success!");
                    }
                }
            }
        }
    } else {
        LOGE("create queue and playState fail!");
    }
    return result;
}

bool OpenSLRender::checkSlresult(SLresult sLresult) {
    if (sLresult == SL_RESULT_SUCCESS) {
        return true;
    }
    return false;
}

int OpenSLRender::GetAudioFrameQueueSize() {
    std::unique_lock<std::mutex> lock(mMutex);
    return mAudioFrameQueue.size();
}

void OpenSLRender::StartRender() {
    while (GetAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !mExit) {
        std::unique_lock<std::mutex> lock(mMutex);
        mCond.wait_for(lock, std::chrono::milliseconds(10));
        lock.unlock();
    }

    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    AudioPlayerCallback(bqPlayerBufferQueue, this);
}

void OpenSLRender::HandleAudioFrameQueue() {
    if (bqPlayerPlay == nullptr) return;

    while (GetAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !mExit) {
        std::unique_lock<std::mutex> lock(mMutex);
        mCond.wait_for(lock, std::chrono::milliseconds(10));
    }

    std::unique_lock<std::mutex> lock(mMutex);
    AudioFrame *audioFrame = mAudioFrameQueue.front();
    if (nullptr != audioFrame && bqPlayerPlay) {
        SLresult result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, audioFrame->data,
                                                          (SLuint32) audioFrame->dataSize);
        if (result == SL_RESULT_SUCCESS) {
//            AudioGLRender::GetInstance()->UpdateAudioFrame(audioFrame);
            mAudioFrameQueue.pop();
            delete audioFrame;
        }

    }
    lock.unlock();

}

void OpenSLRender::CreateSLWaitingThread(OpenSLRender *openSlRender) {
    openSlRender->StartRender();
}

void OpenSLRender::AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    OpenSLRender *openSlRender = static_cast<OpenSLRender *>(context);
    openSlRender->HandleAudioFrameQueue();
}
