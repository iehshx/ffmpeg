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

        result = CreateOutputMixer();
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::Init OutputMixer fail. result=%d", result);
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
    if (mAudioPlayerPlay) {
        if (pData != nullptr && dataSize > 0) {

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


    if (mOutputMixObj) {
        (*mOutputMixObj)->Destroy(mOutputMixObj);
//        outputMixEnvironmentalReverb = nullptr;
        mOutputMixObj = nullptr;
        mBufferQueue = nullptr;
        mAudioPlayerVolume = nullptr;
        bqPlayerEffectSend = nullptr;
    }

    if (mEngineObj) {
        (*mEngineObj)->Destroy(mEngineObj);
        mEngineObj = nullptr;
        mEngineEngine = nullptr;
    }
    if (mAudioPlayerObj) {
        (*mAudioPlayerPlay)->SetPlayState(mAudioPlayerPlay, SL_PLAYSTATE_STOPPED);
        mAudioPlayerObj = nullptr;
        mAudioPlayerPlay = nullptr;
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
    SLresult result = SL_RESULT_SUCCESS;
    do {
        result = slCreateEngine(&mEngineObj, 0, nullptr, 0, nullptr, nullptr);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateEngine slCreateEngine fail. result=%d", result);
            break;
        }

        result = (*mEngineObj)->Realize(mEngineObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateEngine Realize fail. result=%d", result);
            break;
        }

        result = (*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE, &mEngineEngine);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateEngine GetInterface fail. result=%d", result);
            break;
        }

    } while (false);
    return result;
}

bool OpenSLRender::createPlayer() {
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//format type
            (SLuint32) 2,//channel count
            SL_SAMPLINGRATE_44_1,//44100hz
            SL_PCMSAMPLEFORMAT_FIXED_16,// bits per sample
            SL_PCMSAMPLEFORMAT_FIXED_16,// container size
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,// channel mask
            SL_BYTEORDER_LITTLEENDIAN // endianness
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mOutputMixObj};
    SLDataSink slDataSink = {&outputMix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLresult result;

    do {

        result = (*mEngineEngine)->CreateAudioPlayer(mEngineEngine, &mAudioPlayerObj, &slDataSource,
                                                     &slDataSink, 3, ids, req);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateAudioPlayer CreateAudioPlayer fail. result=%d", result);
            break;
        }

        result = (*mAudioPlayerObj)->Realize(mAudioPlayerObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateAudioPlayer Realize fail. result=%d", result);
            break;
        }

        result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_PLAY, &mAudioPlayerPlay);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }

        result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_BUFFERQUEUE,
                                                  &mBufferQueue);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }

        result = (*mBufferQueue)->RegisterCallback(mBufferQueue, AudioPlayerCallback, this);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateAudioPlayer RegisterCallback fail. result=%d", result);
            break;
        }

        result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_VOLUME,
                                                  &mAudioPlayerVolume);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }

    } while (false);

    return result;
}


bool OpenSLRender::CreateOutputMixer() {
    SLresult result = SL_RESULT_SUCCESS;
    do {
        const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
        const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};

        result = (*mEngineEngine)->CreateOutputMix(mEngineEngine, &mOutputMixObj, 1, mids, mreq);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateOutputMixer CreateOutputMix fail. result=%d", result);
            break;
        }

        result = (*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateOutputMixer CreateOutputMix fail. result=%d", result);
            break;
        }

    } while (false);

    return result;
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
    (*mAudioPlayerPlay)->SetPlayState(mAudioPlayerPlay, SL_PLAYSTATE_PLAYING);
    AudioPlayerCallback(mBufferQueue, this);
}

void OpenSLRender::HandleAudioFrameQueue() {
    if (mAudioPlayerPlay == nullptr) return;

    while (GetAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !mExit) {
        std::unique_lock<std::mutex> lock(mMutex);
        mCond.wait_for(lock, std::chrono::milliseconds(10));
    }

    std::unique_lock<std::mutex> lock(mMutex);
    AudioFrame *audioFrame = mAudioFrameQueue.front();
    if (nullptr != audioFrame && mAudioPlayerPlay) {
        if (mBufferQueue) {
            SLresult result = (*mBufferQueue)->Enqueue(mBufferQueue, audioFrame->data,
                                                       (SLuint32) audioFrame->dataSize);
            if (result == SL_RESULT_SUCCESS) {
                if (!mAudioFrameQueue.empty()) {
                    mAudioFrameQueue.pop();
                }
                delete audioFrame;
            }
        }


    }
    lock.unlock();

}

void OpenSLRender::CreateSLWaitingThread(OpenSLRender *openSlRender) {
    openSlRender->StartRender();
}

void OpenSLRender::AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    if (bufferQueue) {
        OpenSLRender *openSlRender = static_cast<OpenSLRender *>(context);
        openSlRender->HandleAudioFrameQueue();
    }
}
