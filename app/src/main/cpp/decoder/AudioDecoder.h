//
// Created by DZ0400351 on 2022/7/28.
//

#ifndef MY_APPLICATION_AUDIODECODER_H
#define MY_APPLICATION_AUDIODECODER_H


extern "C" {
#include <../include/libavutil/samplefmt.h>
#include <../include/libswresample/swresample.h>
#include <../include/libavutil/opt.h>
#include <../include/libavutil/audio_fifo.h>
}

#include <AudioRender.h>
#include "DecoderBase.h"


class AudioDecoder : public DecoderBase {

    // ACC音频一帧采样数
    static const int ACC_NB_SAMPLES = 1024;
    //输出采样格式16bit PCM
    enum AVSampleFormat AUDIO_SAMPLE_FMT = AV_SAMPLE_FMT_S16;
    //输出采样率
    const int AUDIO_DST_SAMPLE_RATE = 44100;
    //通道数 2
    const int AUDIO_DST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
    // 音频编码通道数
    static const int AUDIO_DST_CHANNEL_COUNTS = 2;

public:
    AudioDecoder(const char *url) {
        Init(url, AVMEDIA_TYPE_AUDIO);
    }

    virtual ~AudioDecoder() {
        UnInit();
    }

    void SetAudioRender(AudioRender *audioRender) {
        mAudioRender = audioRender;
    }

private:
    const AVSampleFormat DST_SAMPLT_FORMAT = AV_SAMPLE_FMT_S16;

    SwrContext *mSwrContext = nullptr;

    uint8_t *mAudioOutBuffer = nullptr;
    AudioRender *mAudioRender = nullptr;

    int mNbSamples = 0;

    int mDstFrameDataSze = 0;


    virtual void OnDecoderReady();

    virtual void OnDecoderDone();

    virtual void OnFrameAvailable(AVFrame *frame);

};


#endif //MY_APPLICATION_AUDIODECODER_H
