//
// Created by DZ0400351 on 2022/8/2.
//

#ifndef MY_APPLICATION_NATIVERENDER_H
#define MY_APPLICATION_NATIVERENDER_H

#include "VideoRender.h"
#include <jni.h>
#include "android/native_window.h"
#include "android/native_window_jni.h"

class NativeRender : public VideoRender {
public:
    NativeRender(JNIEnv *env, jobject surface);

    virtual ~NativeRender();

    virtual void Init(int videoWidth, int videoHeight, int *dstSize);

    virtual void RenderVideoFrame(NativeImage *pImage);

    virtual void UnInit();

private:
    ANativeWindow_Buffer mNativeWindowBuffer;
    ANativeWindow *mNativeWindow = nullptr;
    int mDstWidth = 0;
    int mDstHeight = 0;
};


#endif //MY_APPLICATION_NATIVERENDER_H
