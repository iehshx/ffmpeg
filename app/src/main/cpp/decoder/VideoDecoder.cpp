//
// Created by DZ0400351 on 2022/8/2.
//

#include "VideoDecoder.h"
#include "../utils.h"
#include "../MLOG.h"


void VideoDecoder::OnDecoderReady() {
    if (mVideoRender) {
        AVCodecContext *avCodec = GetCodecContext();
        if (avCodec) {

            mSwsContext = sws_alloc_context();
            this->mVideoWidth = avCodec->width;
            this->mVideoHeight = avCodec->height;
            int dstSize[2] = {0};
            mVideoRender->Init(mVideoWidth, mVideoHeight, dstSize);
            this->mRenderWidth = dstSize[0];
            this->mRenderHeight = dstSize[1];
            mRGBAFrame = av_frame_alloc();
            int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, mRenderWidth, mRenderHeight,
                                                      1);
            mFrameBuffer = static_cast<uint8_t *>(av_malloc(bufferSize * sizeof(uint8_t)));
            av_image_fill_arrays(mRGBAFrame->data, mRGBAFrame->linesize, mFrameBuffer,
                                 AV_PIX_FMT_RGBA, mRenderWidth, mRenderHeight, 1);
            mSwsContext = sws_getContext(mVideoWidth, mVideoHeight, avCodec->pix_fmt, mRenderWidth,
                                         mRenderHeight, AV_PIX_FMT_RGBA, SWS_FAST_BILINEAR, nullptr,
                                         nullptr, nullptr);
            LOGE("视频解码准备完毕");
        }
    }
}

void VideoDecoder::OnDecoderDone() {
    LOGE("视频解码完毕");
    if (mVideoRender)
        mVideoRender->UnInit();

    if (mRGBAFrame != nullptr) {
        av_frame_free(&mRGBAFrame);
        mRGBAFrame = nullptr;
    }

    if (mFrameBuffer != nullptr) {
        av_free(mFrameBuffer);
        mFrameBuffer = nullptr;
    }

    if (mSwsContext != nullptr) {
        sws_freeContext(mSwsContext);
        mSwsContext = nullptr;
    }


}

void VideoDecoder::OnFrameAvailable(AVFrame *frame) {

    if (frame) {
        sws_scale(mSwsContext, frame->data, frame->linesize, 0,
                  mVideoHeight, mRGBAFrame->data, mRGBAFrame->linesize);
        NativeImage image;
        image.width = mRenderWidth;
        image.height = mRenderHeight;
        image.ppPlane[0] = mRGBAFrame->data[0];
        image.pLineSize[0] = image.width * 4;
        mVideoRender->RenderVideoFrame(&image);

    }
}
