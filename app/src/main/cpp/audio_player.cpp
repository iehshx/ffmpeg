////
//// Created by DZ0400351 on 2022/7/27.
////
//
//#include "audio_player.h"
//
//void AudioPlayer::init(THREAD_STATE state) {
//    if (state == MAIN) {
//        if (createEngine()) {
//            bool ret = createPlayer();
//            if (ret) {
//                callOnPrepareSuccess(NULL);
//            } else {
//                callOnStateChange(nullptr, STATE_ERROR_CODE_INIT_PLAYER);
//            }
//        } else {
//            callOnStateChange(NULL, STATE_ERROR_CODE_INIT_ENGINE);
//        }
//    } else {
//        JNIEnv *childEnv = NULL;
//        vm->AttachCurrentThread(&childEnv, NULL);
//        if (createEngine()) {
//            bool ret = createPlayer();
//            if (ret) {
//                callOnPrepareSuccess(childEnv);
//            } else {
//                callOnStateChange(childEnv, STATE_ERROR_CODE_INIT_PLAYER);
//            }
//
//        } else {
//            callOnStateChange(childEnv, STATE_ERROR_CODE_INIT_ENGINE);
//        }
//    }
//}
//
//void AudioPlayer::play() {
//    bool decodeRet = createBufferQueueAudioDecoder();
//    bool ret = createBufferQueueAudioPlayer();
//    if (ret) {
//        callOnStateChange(NULL, STATE_CODE_PLAY_SUCCESS);
//    } else {
//        callOnStateChange(NULL, STATE_CODE_PLAY_ERROR);
//    }
//}
//
//bool AudioPlayer::checkSlresult(SLresult result) {
//    if (result == SL_RESULT_SUCCESS) {
//        return true;
//    } else {
//        release();
//        return false;
//    }
//}
//
//bool AudioPlayer::createEngine() {
//    SLresult result = slCreateEngine(&mEngineObj, 0, NULL, 0, NULL, NULL);
//    if (checkSlresult(result)) {
//        result = (*mEngineObj)->Realize(mEngineObj, SL_BOOLEAN_FALSE);
//        if (checkSlresult(result)) {
//            result = (*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE,
//                                                   &mEngineEngine);
//            if (checkSlresult(result)) {
//                const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
//                const SLboolean req[1] = {SL_BOOLEAN_FALSE};
//                result = (*mEngineEngine)->CreateOutputMix(mEngineEngine, &mOutputMixObj, 1, ids,
//                                                          req);
//                if (checkSlresult(result)) {
//                    result = (*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE);
//                    if (checkSlresult(result)) {
//                        result = (*mOutputMixObj)->GetInterface(mOutputMixObj,
//                                                                  SL_IID_ENVIRONMENTALREVERB,
//                                                                  &outputMixEnvironmentalReverb);
//                        if (checkSlresult(result)) {
//                            result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
//                                    outputMixEnvironmentalReverb, &reverbSettings);
//                            LOGE("create AudioPlayer engine success!");
//                            return true;
//                        }
//                    }
//                }
//            }
//        }
//    }
//    LOGE("create AudioPlayer engine faild!");
//    return false;
//}
//
//
////播放的回调 传去queue和 当前引用
//void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
//    if (bq) {
//        AudioPlayer *player = static_cast<AudioPlayer *>(context);
//
//        if (player != NULL && player->sampleRate > 0) {
//            player->current +=
//                    player->bufSize / (player->sampleRate * 2 * 2);
//        if (player->current - player->lastTime > 0.1) {
//            static JNIEnv *env = NULL;
//            player->vm->AttachCurrentThread(&env, nullptr);
//            player->
//                    callOnPlay(env, player
//                    ->current, player->total);
//            player->lastTime = player->current;
//            player->vm->DetachCurrentThread();
//        }
//        }
//    }
//
//
//}
//
//bool AudioPlayer::createBufferQueueAudioPlayer() {
//    SLresult result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_BUFFERQUEUE,
//                                                      &mBufferQueue);
//    if (checkSlresult(result)) {
//        result = (*mBufferQueue)->RegisterCallback(mBufferQueue, bqPlayerCallback,
//                                                          this);
//        if (checkSlresult(result)) {
//            // get the effect send interface
//            bqPlayerEffectSend = nullptr;
//            if (0 == bqPlayerSampleRate) {
//                result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_EFFECTSEND,
//                                                         &bqPlayerEffectSend);
//                if (checkSlresult(result)) {
//                    result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_VOLUME,
//                                                             &mAudioPlayerVolume);
//                    if (checkSlresult(result)) {
//                        // 设置播放器播放状态
//                        result = (*mAudioPlayerPlay)->SetPlayState(mAudioPlayerPlay, SL_PLAYSTATE_PLAYING);
//                        LOGE("create queue and playState success!");
//                        bqPlayerCallback(mBufferQueue, this);
//                        return checkSlresult(result);
//                    }
//                }
//            }
//        }
//    }
//    LOGE("create queue and playState fail!");
//    return false;
//}
//
//bool AudioPlayer::createPlayer() {
//    //支持的资源
//    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
//    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_44_1,
//                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
//                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
//                                   SL_BYTEORDER_LITTLEENDIAN};
//
//    // 声音输出设备
//    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mOutputMixObj};
//    SLDataSink audioSnk = {&loc_outmix, NULL};
//
//    SLDataSource audioSrc = {&loc_bufq, &format_pcm};
//
//    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
//            /*SL_IID_MUTESOLO,*/};
//    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
//            /*SL_BOOLEAN_TRUE,*/ };
//    SLresult result = (*mEngineEngine)->CreateAudioPlayer(mEngineEngine, &mAudioPlayerObj, &audioSrc,
//                                                         &audioSnk,
//                                                         3, ids, req);
//    if (checkSlresult(result)) {
//        result = (*mAudioPlayerObj)->Realize(mAudioPlayerObj, SL_BOOLEAN_FALSE);
//        if (checkSlresult(result)) {
//            result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_PLAY, &mAudioPlayerPlay);
//            if (checkSlresult(result)) {
//                LOGE("create AudioPlayer player success!");
//                return true;
//            }
//        }
//    }
//    LOGE("create AudioPlayer player faild!");
//    return false;
//}
//
//void AudioPlayer::release() {
//    if (mOutputMixObj) {
//        (*mOutputMixObj)->Destroy(mOutputMixObj);
//        outputMixEnvironmentalReverb = nullptr;
//        mOutputMixObj = nullptr;
//        mBufferQueue = nullptr;
//        mAudioPlayerVolume = nullptr;
//        bqPlayerEffectSend = nullptr;
//    }
//
//    if (mEngineObj) {
//        (*mEngineObj)->Destroy(mEngineObj);
//        mEngineObj = nullptr;
//        mEngineEngine = nullptr;
//    }
//    if (mAudioPlayerObj) {
//        (*mAudioPlayerObj)->Destroy(mAudioPlayerObj);
//        mAudioPlayerObj = nullptr;
//        mAudioPlayerPlay = nullptr;
//    }
//
//    if (mAudioPlayerCallBackObj) {
//        this->jniEnv->DeleteGlobalRef(mAudioPlayerCallBackObj);
//        mAudioPlayerCallBackObj = nullptr;
//        audioPlayerCallBackOnPrepareSuccess = nullptr;
//        mAudioPlayerCallBackOnError = nullptr;
//    }
//}
//
//SLVolumeItf AudioPlayer::getVolume() {
//    return mAudioPlayerVolume;
//}
//
//AudioPlayer::~AudioPlayer() {
//    release();
//}
//
//AudioPlayer::AudioPlayer(JNIEnv *jniEnv, jobject pJobject) {
//    this->jniEnv = jniEnv;
//    this->jniEnv->GetJavaVM(&vm);
//    if (!mAudioPlayerCallBackObj) {
//        jclass audioPlayerCallBack = jniEnv->FindClass(
//                "com/example/myapplication/AudioPlayerCallBack");
//        jclass audioPlayer = jniEnv->FindClass(
//                "com/example/myapplication/AudioPlayer");
//        jfieldID callBackFieldId = jniEnv->GetFieldID(audioPlayer, "callback",
//                                                      "Lcom/example/myapplication/AudioPlayerCallBack;");
//        mAudioPlayerCallBackObj = jniEnv->NewGlobalRef(
//                jniEnv->GetObjectField(pJobject, callBackFieldId));
//        if (!mAudioPlayerCallBackOnError) {
//            mAudioPlayerCallBackOnError = jniEnv->GetMethodID(audioPlayerCallBack, "onStateChange",
//                                                             "(I)V");
//        }
//        if (!audioPlayerCallBackOnPrepareSuccess) {
//            audioPlayerCallBackOnPrepareSuccess = jniEnv->GetMethodID(audioPlayerCallBack,
//                                                                      "onPrepareSuccess",
//                                                                      "()V");
//        }
//        if (!mAudioPlayerCallBackOnPlayProcess) {
//            mAudioPlayerCallBackOnPlayProcess = jniEnv->GetMethodID(audioPlayerCallBack,
//                                                            "onPlay",
//                                                            "(II)V");
//        }
//    }
//
//}
//
//void AudioPlayer::callOnStateChange(JNIEnv *env, int code) {
//    if (mAudioPlayerCallBackObj && mAudioPlayerCallBackOnError) {
//        if (env != NULL) {
//            env->CallVoidMethod(mAudioPlayerCallBackObj, mAudioPlayerCallBackOnError, code);
//        } else {
//            jniEnv->CallVoidMethod(mAudioPlayerCallBackObj, mAudioPlayerCallBackOnError, code);
//        }
//    }
//}
//
//void AudioPlayer::callOnPrepareSuccess(JNIEnv *env) {
//    if (mAudioPlayerCallBackObj && audioPlayerCallBackOnPrepareSuccess) {
//        if (env != NULL) {
//            env->CallVoidMethod(mAudioPlayerCallBackObj, audioPlayerCallBackOnPrepareSuccess);
//        } else {
//            jniEnv->CallVoidMethod(mAudioPlayerCallBackObj, audioPlayerCallBackOnPrepareSuccess);
//        }
//    }
//}
//
//void AudioPlayer::setAudioParams(int sampleRate, int bufSize) {
//    this->sampleRate = sampleRate;
//    this->bufSize = bufSize;
//}
//
//void AudioPlayer::equeue(uint8_t *buffer) {
//    if (buffer) {
//        SLresult result = (*mBufferQueue)->Enqueue(mBufferQueue, buffer, bufSize);
//    }
//}
//
//void AudioPlayer::stop() {
//    if (mAudioPlayerPlay) {
//        (*mAudioPlayerPlay)->SetPlayState(mAudioPlayerPlay, false);
//    }
//}
//
//void AudioPlayer::callOnPlay(JNIEnv *env, int current, int total) {
//    if (mAudioPlayerCallBackObj && mAudioPlayerCallBackOnPlayProcess) {
//        if (env != NULL) {
//            env->CallVoidMethod(mAudioPlayerCallBackObj, mAudioPlayerCallBackOnPlayProcess, current, total);
//        } else {
//            jniEnv->CallVoidMethod(mAudioPlayerCallBackObj, mAudioPlayerCallBackOnPlayProcess, current,
//                                   total);
//        }
//    }
//}
//
//bool AudioPlayer::createBufferQueueAudioDecoder(pContext:) {
//    return false;
//}
