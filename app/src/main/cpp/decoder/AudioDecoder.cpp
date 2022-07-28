//
// Created by DZ0400351 on 2022/7/28.
//

#include "AudioDecoder.h"

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

            mNbSamples = (int) av_rescale_rnd(ACC_NB_SAMPLES, AUDIO_DST_SAMPLE_RATE,
                                              codeCtx->sample_rate,
                                              AV_ROUND_UP);
            mDstFrameDataSze = av_samples_get_buffer_size(NULL, codeCtx->channels,
                                                          mNbSamples,
                                                          DST_SAMPLT_FORMAT, 1);
            mAudioOutBuffer = static_cast<uint8_t *>(av_malloc(mDstFrameDataSze));
            mAudioRender->Init();
        }
    }
}

void AudioDecoder::OnDecoderDone() {
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
//        mNbSamples = frame->nb_samples;
        if (result > 0) {
            mAudioRender->RenderAudioFrame(mAudioOutBuffer, mDstFrameDataSze);
        }
    }
}
