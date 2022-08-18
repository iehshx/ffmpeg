//
// Created by DZ0400351 on 2022/8/17.
//

#include <ext/matrix_transform.hpp>
#include <ext/matrix_clip_space.hpp>
#include "GLRender.h"
#include "../MLOG.h"
#include "../GLUtils.h"


GLRender *GLRender::s_Instance = nullptr;
std::mutex GLRender::m_Mutex;


void GLRender::Init(int videoWidth, int videoHeight, int *dstSize) {
    if (dstSize) {
        dstSize[0] = videoWidth;
        dstSize[1] = videoHeight;
    }
    mFrameIndex = 0;
    UpdateMVPMatrix(0, 0, 1.0f, 1.0f);//纹理坐标
}

void GLRender::RenderVideoFrame(NativeImage *pImage) {
    LOGE("GLRender::RenderVideoFrame pImage=%p", pImage);
    if (pImage == nullptr || pImage->ppPlane[0] == nullptr)
        return;
    std::unique_lock<std::mutex> lock(m_Mutex);
    if (pImage->width != mRenderImage.width || pImage->height != mRenderImage.height) {
        if (mRenderImage.ppPlane[0] != nullptr) {
            NativeImageUtil::FreeNativeImage(&mRenderImage);
        }
        memset(&mRenderImage, 0, sizeof(NativeImage));
        mRenderImage.format = pImage->format;
        mRenderImage.width = pImage->width;
        mRenderImage.height = pImage->height;
        NativeImageUtil::AllocNativeImage(&mRenderImage);
    }

    NativeImageUtil::CopyNativeImage(pImage, &mRenderImage);
}

void GLRender::UnInit() {

}

//顶点着色器
static char vShaderStr[] =
        "#version 300 es\n"
        "layout(location = 0) in vec4 a_position;\n"
        "layout(location = 1) in vec2 a_texCoord;\n"
        "uniform mat4 u_MVPMatrix;\n"
        "out vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = u_MVPMatrix * a_position;\n"
        "    v_texCoord = a_texCoord;\n"
        "}";
//片元着色器
static char fShaderStr[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_texture0;\n"
        "uniform sampler2D s_texture1;\n"
        "uniform sampler2D s_texture2;\n"
        "uniform int u_nImgType;// 1:RGBA"
        "\n"
        "void main()\n"
        "{\n"
        " outColor = texture(s_texture0, v_texCoord);\n"
        "}";

//片元着色器
static char fColorShaderStr[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_texture0;\n"
        "uniform sampler2D s_texture1;\n"
        "uniform sampler2D s_texture2;\n"
        "uniform int u_nImgType;// 1:RGBA"
        "\n"
        "void main()\n"
        "{\n"
        "float y = v_texCoord.y;\n"
        " outColor = texture(s_texture0,vec2(v_texCoord));\n"
        "if(y<0.5){\n"
        "y+=0.5;\n"
        " outColor = vec4(vec3(outColor.r * 0.3,outColor.g * 0.59,outColor.b * 0.11),outColor.a);\n"
        "}else{\n"
        "y-=0.5;\n"
        "}\n"
        "}";
//片元着色器
static char fSpliteShaderStr[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_texture0;\n"
        "uniform sampler2D s_texture1;\n"
        "uniform sampler2D s_texture2;\n"
        "uniform int u_nImgType;// 1:RGBA"
        "\n"
        "void main()\n"
        "{\n"
        "float x = v_texCoord.x;\n"
        "if(x<0.5){\n"
        "x+=0.25;\n"
        "}else{\n"
        "x-=0.25;\n"
        "}\n"
        " outColor = texture(s_texture0,vec2(x,v_texCoord.y));\n"
        "}";

GLfloat verticesCoords[] = {
        -1.0f, 1.0f, 0.0f,  // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f, -1.0f, 0.0f,  // Position 2
        1.0f, 1.0f, 0.0f,  // Position 3
};
GLfloat textureCoords[] = {
        0.0f, 0.0f,        // TexCoord 0
        0.0f, 1.0f,        // TexCoord 1
        1.0f, 1.0f,        // TexCoord 2
        1.0f, 0.0f         // TexCoord 3
};
GLushort indices[] = {0, 1, 2, 0, 2, 3};

void GLRender::OnSurfaceCreated(int renderStyle) {

    LOGE("GLRender::OnSurfaceCreated");
    char *shaderStr = fShaderStr;
    switch (renderStyle) {
        case 0:
            shaderStr = fShaderStr;
            break;
        case 1:
            shaderStr = fSpliteShaderStr;
            break;
        case 2:
            shaderStr = fColorShaderStr;
            break;
    }
    mProgramObj = GLUtils::CreateProgram(vShaderStr, shaderStr);
    if (!mProgramObj) {
        LOGE("mProgramObj fail");
        return;
    }
    glGenTextures(TEXTURE_NUM, mTextureIds);
    for (int i = 0; i < TEXTURE_NUM; ++i) {//激活1~3的layer
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, mTextureIds[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);

//    m_TouchXY = vec2(0.5f, 0.5f);
}

void GLRender::OnSurfaceChanged(int w, int h) {
    LOGE("GLRender::OnSurfaceChanged [w, h]=[%d, %d]", w, h);
    m_ScreenSize.x = w;
    m_ScreenSize.y = h;
    glViewport(0, 0, w, h);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void GLRender::OnDrawFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (mProgramObj == GL_NONE || mTextureIds == GL_NONE ||
        mRenderImage.ppPlane[0] == nullptr)
        return;
    LOGE("GLRender::OnDrawFrame [w, h]=[%d, %d]", mRenderImage.width, mRenderImage.height);
    mFrameIndex++;
    //加互斥锁，解码线程和渲染线程是 2 个不同的线程，避免数据访问冲突
    std::unique_lock<std::mutex> lock(m_Mutex);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureIds[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mRenderImage.width, mRenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, mRenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    lock.unlock();
// Use the program object
    glUseProgram(mProgramObj);

    glBindVertexArray(m_VaoId);

    GLUtils::setMat4(mProgramObj, "u_MVPMatrix", m_MVPMatrix);

    for (int i = 0; i < TEXTURE_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, mTextureIds[i]);
        char samplerName[64] = {0};
        sprintf(samplerName, "s_texture%d", i);
        GLUtils::setInt(mProgramObj, samplerName, i);
    }

    //float time = static_cast<float>(fmod(m_FrameIndex, 60) / 50);
    //GLUtils::setFloat(m_ProgramObj, "u_Time", time);

    float offset = (sin(mFrameIndex * MATH_PI / 40) + 1.0f) / 2.0f;
    GLUtils::setFloat(mProgramObj, "u_Offset", offset);
    GLUtils::setVec2(mProgramObj, "u_TexSize", glm::vec2(mRenderImage.width, mRenderImage.height));
    GLUtils::setInt(mProgramObj, "u_nImgType", mRenderImage.format);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);
}

void GLRender::UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY) {
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scaleX, scaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    m_MVPMatrix = Projection * View * Model;

}

void GLRender::UpdateMVPMatrix(TransformMatrix *pTransformMatrix) {
    //BaseGLRender::UpdateMVPMatrix(pTransformMatrix);
//    float fFactorX = 1.0f;
//    float fFactorY = 1.0f;
//
//    if (pTransformMatrix->mirror == 1) {
//        fFactorX = -1.0f;
//    } else if (pTransformMatrix->mirror == 2) {
//        fFactorY = -1.0f;
//    }
//
//    float fRotate = MATH_PI * pTransformMatrix->degree * 1.0f / 180;
//    if (pTransformMatrix->mirror == 0) {
//        if (pTransformMatrix->degree == 270) {
//            fRotate = MATH_PI * 0.5;
//        } else if (pTransformMatrix->degree == 180) {
//            fRotate = MATH_PI;
//        } else if (pTransformMatrix->degree == 90) {
//            fRotate = MATH_PI * 1.5;
//        }
//    } else if (pTransformMatrix->mirror == 1) {
//        if (pTransformMatrix->degree == 90) {
//            fRotate = MATH_PI * 0.5;
//        } else if (pTransformMatrix->degree == 180) {
//            fRotate = MATH_PI;
//        } else if (pTransformMatrix->degree == 270) {
//            fRotate = MATH_PI * 1.5;
//        }
//    }
//
//    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
//    glm::mat4 View = glm::lookAt(
//            glm::vec3(0, 0, 1), // Camera is at (0,0,1), in World Space
//            glm::vec3(0, 0, 0), // and looks at the origin
//            glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
//    );
//
//    // Model matrix : an identity matrix (model will be at the origin)
//    glm::mat4 Model = glm::mat4(1.0f);
//    Model = glm::scale(Model, glm::vec3(fFactorX * pTransformMatrix->scaleX,
//                                        fFactorY * pTransformMatrix->scaleY, 1.0f));
//    Model = glm::rotate(Model, fRotate, glm::vec3(0.0f, 0.0f, 1.0f));
//    Model = glm::translate(Model,
//                           glm::vec3(pTransformMatrix->translateX, pTransformMatrix->translateY, 0.0f));
//
//    LOGE("VideoGLRender::UpdateMVPMatrix rotate %d,%.2f,%0.5f,%0.5f,%0.5f,%0.5f,", pTransformMatrix->degree, fRotate,
//            pTransformMatrix->translateX, pTransformMatrix->translateY,
//            fFactorX * pTransformMatrix->scaleX, fFactorY * pTransformMatrix->scaleY);
//
//    m_MVPMatrix = Projection * View * Model;
}

void GLRender::ReleaseInstance() {
    if (s_Instance != nullptr) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (s_Instance != nullptr) {
            delete s_Instance;
            s_Instance = nullptr;
        }
    }
}

GLRender *GLRender::GetInstance() {
    if (s_Instance == nullptr) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (s_Instance == nullptr) {
            s_Instance = new GLRender();
        }

    }
    return s_Instance;
}

GLRender::GLRender() {

}

GLRender::~GLRender() {
    NativeImageUtil::FreeNativeImage(&mRenderImage);
}

