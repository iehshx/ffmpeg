//
// Created by DZ0400351 on 2022/8/2.
//

#include <cstring>
#include "NativeRender.h"

NativeRender::NativeRender(JNIEnv *env, jobject surface) {
    mNativeWindow = ANativeWindow_fromSurface(env, surface);
}

NativeRender::~NativeRender() {
    if (mNativeWindow)
        ANativeWindow_release(mNativeWindow);
}

void NativeRender::Init(int videoWidth, int videoHeight, int *dstSize) {
    if (mNativeWindow == nullptr) {
        return;
    }

    int windowWidth = ANativeWindow_getWidth(mNativeWindow);
    int windowHeight = ANativeWindow_getWidth(mNativeWindow);

    if (windowHeight < windowHeight * videoWidth / videoHeight) {
        mDstWidth = windowWidth;
        mDstHeight = windowHeight * videoHeight / videoWidth;
    } else {
        mDstWidth = windowHeight * videoWidth / videoHeight;
        mDstHeight = windowHeight;
    }

    ANativeWindow_setBuffersGeometry(mNativeWindow, mDstWidth,
                                     mDstHeight, WINDOW_FORMAT_RGBA_8888);
    dstSize[0] = mDstWidth;
    dstSize[1] = mDstHeight;
}

void NativeRender::RenderVideoFrame(NativeImage *pImage) {
    LOGE("NativeRender::RenderVideoFrame pImage=%p", pImage);
    if (mNativeWindow == nullptr || pImage == nullptr) return;
    ANativeWindow_lock(mNativeWindow, &mNativeWindowBuffer, nullptr);
    uint8_t *dstBuffer = static_cast<uint8_t *>(mNativeWindowBuffer.bits);

    int srcLineSize = pImage->width * 4;//RGBA
    int dstLineSize = mNativeWindowBuffer.stride * 4;

    for (int i = 0; i < mDstHeight; ++i) {
        memcpy(dstBuffer + i * dstLineSize, pImage->ppPlane[0] + i * srcLineSize, srcLineSize);
    }

    ANativeWindow_unlockAndPost(mNativeWindow);
}

void NativeRender::UnInit() {

}
