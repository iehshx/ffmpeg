//
// Created by DZ0400351 on 2022/7/28.
//

#include "DecoderBase.h"
#include "../MLOG.h"
#include "../utils.h"

int DecoderBase::InitFFDecoder() {
    //创建上下文
    int ret = -1;
    do {
        if (mUrl) {
            mAVFormatContext = avformat_alloc_context();
            int avFormatContextCode = 0;
            avFormatContextCode = avformat_open_input(&mAVFormatContext, mUrl, nullptr,
                                                      nullptr);
            if (avFormatContextCode != 0) {
                char *err = av_err2str(avFormatContextCode);
                LOGE("avFormatContextCode error = %s", err);
                break;
            }
            int streamInfoCode = avformat_find_stream_info(mAVFormatContext, nullptr);
            if (streamInfoCode < 0) {
                char *err = av_err2str(streamInfoCode);
                LOGE("avFormatContextCode error = %s", err);
                break;
            }
            //获取流信息
            for (int i = 0; i < mAVFormatContext->nb_streams; ++i) {
                if (mAVFormatContext->streams[i]->codecpar->codec_type == mediaType) {
                    mStreamIndex = i;
                    break;
                }
            }
            if (mStreamIndex == -1) {
                LOGE("mStreamIndex get Fail,mediaType = %d", mediaType);
                break;
            }
            AVStream *stream = mAVFormatContext->streams[mStreamIndex];
            AVCodec *avCodec = avcodec_find_decoder(stream->codecpar->codec_id);
            mAVCodecContext = avcodec_alloc_context3(avCodec);
            avcodec_parameters_to_context(mAVCodecContext, stream->codecpar);
            int codecCode = avcodec_open2(mAVCodecContext, avCodec, nullptr);
            if (codecCode != 0) {
                char *err = av_err2str(codecCode);
                LOGE("avFormatContextCode error = %s", err);
                break;
            }
            LOGE("采样率是:%d,声道数是:%d", mAVCodecContext->sample_rate,
                 mAVCodecContext->channels);
            //输入的采样格式
            mDuration = mAVFormatContext->duration / AV_TIME_BASE * 1000;//统计全局时间
            mAvPacket = av_packet_alloc();
            mAvFrame = av_frame_alloc();
            ret = 0;
        }
    } while (false);
    return ret;
}

void DecoderBase::UnInitDecoder() {
    if (mAvFrame) {
        av_frame_free(&mAvFrame);
        mAvFrame = nullptr;
    }
    if (mAvPacket) {
        av_packet_free(&mAvPacket);
        mAvPacket = nullptr;
    }
    if (mAVCodecContext) {
        avcodec_close(mAVCodecContext);
        avcodec_free_context(&mAVCodecContext);
        mAVCodecContext = nullptr;
    }
    if (mAVFormatContext) {
        avformat_close_input(&mAVFormatContext);
        avformat_free_context(mAVFormatContext);
        mAVFormatContext = nullptr;
    }
}

void DecoderBase::DoAVDecoding(DecoderBase *decoder) {
    do {
        if (decoder->InitFFDecoder() != 0) {
            break;
        }
        decoder->OnDecoderReady();
        decoder->DecodingLoop();
    } while (false);
    decoder->UnInitDecoder();
    decoder->OnDecoderDone();
}


void DecoderBase::StartDecodingThread() {
    mThread = new thread(DoAVDecoding, this);
}

//同步状态 todo
void DecoderBase::DecodingLoop() {
    {

        std::unique_lock<std::mutex> lock(mMutex);
        mDecoderState = STATE_DECODING;
        lock.unlock();

    }

    while (true) {
        while (mDecoderState == STATE_PAUSE) {
            std::unique_lock<std::mutex> lock(mMutex);
            mCond.wait_for(lock, std::chrono::milliseconds(10));
            mStartTimeStamp = GetSysCurrentTime() - mCurTimeStamp;
        }
        if (mDecoderState == STATE_STOP) {
            break;
        }
        if (mStartTimeStamp == -1)
            mStartTimeStamp = GetSysCurrentTime();

        if (DecodeOnePacket() != 0) {
            std::unique_lock<std::mutex> lock(mMutex);
            mDecoderState = STATE_PAUSE;
        }
    }

}

void DecoderBase::UpdateTimeStamp() {

    std::unique_lock<std::mutex> lock(mMutex);
    if (mAvFrame->pkt_dts != AV_NOPTS_VALUE) {
        mCurTimeStamp = mAvFrame->pkt_dts;
    } else if (mAvFrame->pts != AV_NOPTS_VALUE) {
        mCurTimeStamp = mAvFrame->pts;
    } else {
        mCurTimeStamp = 0;
    }
    mCurTimeStamp = (int64_t) (
            (mCurTimeStamp * av_q2d(mAVFormatContext->streams[mStreamIndex]->time_base)) * 1000);
}

int DecoderBase::DecodeOnePacket() {
    int ret = av_read_frame(mAVFormatContext, mAvPacket);
    while (ret == 0) {
        if (mAvPacket->stream_index == mStreamIndex) {
            if (avcodec_send_packet(mAVCodecContext, mAvPacket) == AVERROR_EOF) {
                ret = -1;
                av_packet_unref(mAvPacket);
                break;
            }
            int frameCount = 0;

            while (avcodec_receive_frame(mAVCodecContext, mAvFrame) == 0) {
                UpdateTimeStamp();
                AVSync();
                OnFrameAvailable(mAvFrame);
                frameCount++;
            }
            if (frameCount > 0) {
                ret = 0;
                av_packet_unref(mAvPacket);
                break;
            }
            av_packet_unref(mAvPacket);
            ret = av_read_frame(mAVFormatContext, mAvPacket);
        }
    }
    return ret;
}

long DecoderBase::AVSync() {
    long curSysTime = GetSysCurrentTime();
    //基于系统时钟计算从开始播放流逝的时间
    long elapsedTime = curSysTime - mStartTimeStamp;

    if (mediaType == AVMEDIA_TYPE_AUDIO) {
        mMsgContext->onProcess(mCurTimeStamp * 1.0f / 1000);
    }
//    if(m_MsgContext && m_MsgCallback && m_MediaType == AVMEDIA_TYPE_AUDIO)
//        m_MsgCallback(m_MsgContext, MSG_DECODING_TIME, m_CurTimeStamp * 1.0f / 1000);

    long delay = 0;

    //向系统时钟同步
    if (mCurTimeStamp > elapsedTime) {
        //休眠时间
        auto sleepTime = static_cast<unsigned int>(mCurTimeStamp - elapsedTime);//ms
        //限制休眠时间不能过长
        sleepTime = sleepTime > DELAY_THRESHOLD ? DELAY_THRESHOLD : sleepTime;
        av_usleep(sleepTime * 1000);
    }
    delay = elapsedTime - mCurTimeStamp;
    return 0;
}

void DecoderBase::Start() {
    if (mThread == nullptr) {
        StartDecodingThread();
    } else {
        std::unique_lock<std::mutex> lock(mMutex);
        mDecoderState = STATE_DECODING;
        mCond.notify_all();
    }
}

void DecoderBase::Pause() {
    std::unique_lock<std::mutex> lock(mMutex);
    mDecoderState = STATE_PAUSE;
}

void DecoderBase::Stop() {
    std::unique_lock<std::mutex> lock(mMutex);
    mDecoderState = STATE_STOP;
    mCond.notify_all();
}

int DecoderBase::Init(const char *url, AVMediaType mediaType) {
    strcpy(mUrl, url);
    this->mediaType = mediaType;
    return 0;
}

void DecoderBase::UnInit() {
    if (mThread) {
        Stop();
        mThread->join();
        delete mThread;
        mThread = nullptr;
    }
}

float DecoderBase::GetCurrentPosition() {
    return 0;
}

void DecoderBase::SeekToPosition(float position) {

}

