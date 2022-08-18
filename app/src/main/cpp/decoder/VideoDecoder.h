//
// Created by DZ0400351 on 2022/8/2.
//

#ifndef MY_APPLICATION_VIDEODECODER_H
#define MY_APPLICATION_VIDEODECODER_H
extern "C" {
#include "../include/libswscale/swscale.h"
};

#include "DecoderBase.h"
#include <cstdint>
#include <VideoRender.h>

class VideoDecoder : public DecoderBase {

public:
    VideoDecoder(const char *url) {
        Init(url, AVMEDIA_TYPE_VIDEO);
    }

    virtual ~VideoDecoder() {
        UnInit();
    }

    void SetVideoRender(VideoRender *videoRender) {
        this->mVideoRender = videoRender;
    }

private:
    VideoRender *mVideoRender;
    SwsContext *mSwsContext = nullptr;
    AVFrame *mRGBAFrame = nullptr;


    virtual void OnDecoderReady();

    virtual void OnDecoderDone();

    virtual void OnFrameAvailable(AVFrame *frame);

    int mVideoWidth = 0;
    int mVideoHeight = 0;
    int mRenderWidth = 0;
    int mRenderHeight = 0;
    uint8_t *mFrameBuffer = nullptr;
};


#endif //MY_APPLICATION_VIDEODECODER_H
