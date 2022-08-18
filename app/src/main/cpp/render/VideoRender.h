//
// Created by DZ0400351 on 2022/8/2.
//

#ifndef MY_APPLICATION_VIDEORENDER_H
#define MY_APPLICATION_VIDEORENDER_H

#define VIDEO_RENDER_OPENGL             0
#define VIDEO_RENDER_ANWINDOW           1

#include "../utils.h"

class VideoRender {
public:
    VideoRender() {
    }

    virtual ~VideoRender() {}

    virtual void Init(int videoWidth, int videoHeight, int *dstSize) = 0;

    virtual void RenderVideoFrame(NativeImage *pImage) = 0;

    virtual void UnInit() = 0;

};


#endif //MY_APPLICATION_VIDEORENDER_H
