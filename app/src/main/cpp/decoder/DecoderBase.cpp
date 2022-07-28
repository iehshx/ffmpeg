//
// Created by DZ0400351 on 2022/7/28.
//

#include "DecoderBase.h"
#include "../MLOG.h"

int DecoderBase::InitFFDecoder() {
    //创建上下文
    int ret = -1;
    if (mUrl) {
        mAVFormatContext = avformat_alloc_context();
        int avFormatContextCode = 0;
        avFormatContextCode = avformat_open_input(&mAVFormatContext, mUrl, nullptr,
                                                  nullptr);
        if (avFormatContextCode != 0) {
            char *err = av_err2str(avFormatContextCode);
            LOGE("avFormatContextCode error = %s", err);
        } else {
            int streamInfoCode = avformat_find_stream_info(mAVFormatContext, nullptr);
            if (streamInfoCode < 0) {
                char *err = av_err2str(streamInfoCode);
                LOGE("avFormatContextCode error = %s", err);
            } else {
                //获取流信息
                for (int i = 0; i < mAVFormatContext->nb_streams; ++i) {
                    if (mAVFormatContext->streams[i]->codecpar->codec_type == mediaType) {
                        mStreamIndex = i;
                        break;
                    }
                }
                if (mStreamIndex == -1) {
                    LOGE("mStreamIndex get Fail,mediaType = %d", mediaType);
                }
                AVStream *stream = mAVFormatContext->streams[mStreamIndex];
                AVCodec *avCodec = avcodec_find_decoder(stream->codecpar->codec_id);
                mAVCodecContext = avcodec_alloc_context3(avCodec);
                avcodec_parameters_to_context(mAVCodecContext, stream->codecpar);
                int codecCode = avcodec_open2(mAVCodecContext, avCodec, nullptr);
                if (codecCode != 0) {
                    char *err = av_err2str(codecCode);
                    LOGE("avFormatContextCode error = %s", err);
                }
                LOGE("采样率是:%d,声道数是:%d", mAVCodecContext->sample_rate,
                     mAVCodecContext->channels);
                //输入的采样格式
                mDuration = mAVFormatContext->duration / AV_TIME_BASE * 1000;//统计全局时间
                mAvPacket = av_packet_alloc();
                mAvFrame = av_frame_alloc();
                ret = 0;
            }
        }
    }
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
    if (DecodeOnePacket() != 0) {
        //解码结束 暂停
    }
}

void DecoderBase::UpdateTimeStamp() {

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
    int frameCount = 0;
    while (ret == 0) {
        if (mAvPacket->stream_index == mStreamIndex) {
            if (avcodec_send_packet(mAVCodecContext, mAvPacket) == AVERROR_EOF) {
                ret = -1;
                av_packet_unref(mAvPacket);
                break;
            }
            while (avcodec_receive_frame(mAVCodecContext, mAvFrame) == 0) {
                UpdateTimeStamp();
                AVSync();
                OnFrameAvailable(mAvFrame);
                frameCount++;
            }
            if (frameCount > 0) {
                ret = 0;
            }
            av_packet_unref(mAvPacket);
            ret = av_read_frame(mAVFormatContext, mAvPacket);
        }
    }
    LOGE("解码%d。", frameCount);
    return ret;
}

long DecoderBase::AVSync() {
    return 0;
}

void DecoderBase::Start() {
    if (mThread == nullptr) {
        StartDecodingThread();
    }
}

void DecoderBase::Pause() {

}

void DecoderBase::Stop() {

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
