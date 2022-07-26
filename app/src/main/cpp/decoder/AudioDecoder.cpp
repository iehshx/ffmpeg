//
// Created by DZ0400351 on 2022/7/28.
//

#include "AudioDecoder.h"
#include "../MLOG.h"

void AudioDecoder::OnDecoderReady() {

    if (mAudioRender) {

        AVCodecContext *codeCtx = GetCodecContext();
        if (codeCtx) {
            mSwrContext = swr_alloc();
            swr_alloc_set_opts(mSwrContext,
                               AUDIO_DST_CHANNEL_LAYOUT, DST_SAMPLT_FORMAT, AUDIO_DST_SAMPLE_RATE,
                               codeCtx->channel_layout, codeCtx->sample_fmt, codeCtx->sample_rate,
                               0, NULL);
            swr_init(mSwrContext);

//            mNbSamples = 1152;
            //a = 1152  b = 44100 c = 44100
            //重采样后输出的样本数  = 原本的采样率 * 输出的采样率 / 输入的采样率
            //转换公式 inCount / inSampleRate = outCount/outSampleRate
            //outCount = inCount/inSampleRate * outSampleRate
            mNbSamples = (int) av_rescale_rnd(
                    codeCtx->codec_id == AV_CODEC_ID_MP3 ? MP3_NB_SAMPLES : ACC_NB_SAMPLES,
                    AUDIO_DST_SAMPLE_RATE,
                    codeCtx->sample_rate, AV_ROUND_UP);
            mDstFrameDataSze = av_samples_get_buffer_size(NULL, AUDIO_DST_CHANNEL_COUNTS,
                                                          mNbSamples, DST_SAMPLT_FORMAT, 1);
            LOGE("AudioDecoder::OnDecoderReady [mNbSamples, mDstFrameDataSze]=[%d, %d]",
                 mNbSamples, mDstFrameDataSze);
            mAudioOutBuffer = (uint8_t *) malloc(mDstFrameDataSze);
            mAudioRender->Init();
            LOGE("AudioDecoder::OnDecoderReady 音频解码准备完毕");
        }
    }
}

void AudioDecoder::OnDecoderDone() {
    LOGE("音频解码完毕");
    if (mAudioRender)
        mAudioRender->UnInit();

    if (mAudioOutBuffer) {
        av_free(mAudioOutBuffer);
        mAudioOutBuffer = nullptr;
    }

    if (mSwrContext) {
        swr_free(&mSwrContext);
        mSwrContext = nullptr;
    }
}

void AudioDecoder::OnFrameAvailable(AVFrame *frame) {
    if (mAudioRender) {
        int result = swr_convert(mSwrContext, &mAudioOutBuffer, mDstFrameDataSze / 2,
                                 (const uint8_t **) frame->data, frame->nb_samples);
        if (result > 0) {
            mAudioRender->RenderAudioFrame(mAudioOutBuffer, mDstFrameDataSze);
        }
    }
}
