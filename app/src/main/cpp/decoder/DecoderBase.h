//
// Created by DZ0400351 on 2022/7/28.
//

#ifndef MY_APPLICATION_DECODERBASE_H
#define MY_APPLICATION_DECODERBASE_H

extern "C" {
#include <../include/libavcodec/avcodec.h>
#include <../include/libavformat/avformat.h>
#include <../include/libavutil/frame.h>
#include <../include/libavutil/time.h>
#include <../include/libavcodec/jni.h>
}

#include "Decoder.h"
#include <thread>
#include <MediaPlayer.h>

using namespace std;

enum DecoderState {
    STATE_UNKNOWN,
    STATE_DECODING,
    STATE_PAUSE,
    STATE_STOP
};

class DecoderBase : public Decoder {
public:
    DecoderBase() {};

    virtual~ DecoderBase() {};

    //开始播放
    virtual void Start();

    //暂停播放
    virtual void Pause();

    //停止
    virtual void Stop();

    //获取时长
    virtual float GetDuration() {
        //ms to s
        return mDuration * 1.0f / 1000;
    }

    virtual void SetMessageCallback(void *context) {
        mMsgContext = static_cast<MediaPlayer *>(context);
    }

    //seek 到某个时间点播放
    virtual void SeekToPosition(float position);

    //当前播放的位置，用于更新进度条和音视频同步
    virtual float GetCurrentPosition();

protected:
    virtual int Init(const char *url, AVMediaType mediaType);

    virtual void UnInit();

    virtual void OnDecoderReady() = 0;

    virtual void OnDecoderDone() = 0;

    //解码数据的回调
    virtual void OnFrameAvailable(AVFrame *frame) = 0;

    AVCodecContext *GetCodecContext() {
        return mAVCodecContext;
    }


private:
    int InitFFDecoder();

    void UnInitDecoder();

    //启动解码线程
    void StartDecodingThread();

    //音视频解码循环
    void DecodingLoop();

    //更新显示时间戳
    void UpdateTimeStamp();

    //音视频同步
    long AVSync();

    //解码一个packet编码数据
    int DecodeOnePacket();


    //线程调用
    static void DoAVDecoding(DecoderBase *decoder);

    //封装格式上下文
    AVFormatContext *mAVFormatContext = nullptr;
    //解码器上下文
    AVCodecContext *mAVCodecContext = nullptr;
    //解码器
    AVCodec *aVCodec = nullptr;
    //编码的数据包
    AVPacket *mAvPacket = nullptr;
    //解码的帧
    AVFrame *mAvFrame = nullptr;
    //数据流的类型
    AVMediaType mediaType = AVMEDIA_TYPE_UNKNOWN;
    char mUrl[2048] = {0};
    long mCurTimeStamp = 0;
    long mStartTimeStamp = -1;
    long mDuration = 0;
    int mStreamIndex = -1;
    thread *mThread = nullptr;
    mutex mMutex;
    condition_variable mCond;
    MediaPlayer *mMsgContext;
    volatile int mDecoderState = STATE_UNKNOWN;
};


#endif //MY_APPLICATION_DECODERBASE_H
