//
// Created by DZ0400351 on 2022/8/17.
//

#ifndef MY_APPLICATION_GLRENDER_H
#define MY_APPLICATION_GLRENDER_H

#include "VideoRender.h"
#include <thread>
#include "BaseGLRender.h"
#include "stdlib.h"
#include <vec2.hpp>
#include <ext/vector_int2.hpp>
#include <GLES3/gl3.h>
#include <detail/type_mat4x4.hpp>

#define MATH_PI 3.1415926535897932384626433832802
#define TEXTURE_NUM 3

class GLRender : public VideoRender, public BaseGLRender {
public:
    virtual void Init(int videoWidth, int videoHeight, int *dstSize);

    virtual void RenderVideoFrame(NativeImage *pImage);

    virtual void UnInit();

    virtual void OnSurfaceCreated(int renderStyle);

    virtual void OnSurfaceChanged(int w, int h);

    virtual void OnDrawFrame();

    static GLRender *GetInstance();

    static void ReleaseInstance();

    virtual void UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY);

    virtual void UpdateMVPMatrix(TransformMatrix *pTransformMatrix);
//    virtual void SetTouchLoc(float touchX, float touchY) {
//        m_TouchXY.x = touchX / m_ScreenSize.x;
//        m_TouchXY.y = touchY / m_ScreenSize.y;
//    }

private:
    GLRender();

    virtual ~GLRender();

    static std::mutex m_Mutex;
    static GLRender *s_Instance;
    GLuint mProgramObj = GL_NONE;
    GLuint mTextureIds[TEXTURE_NUM];
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    NativeImage mRenderImage;
    glm::mat4 m_MVPMatrix;

    int mFrameIndex;
    glm::vec2 m_TouchXY;
    glm::vec2 m_ScreenSize;
};


#endif //MY_APPLICATION_GLRENDER_H
