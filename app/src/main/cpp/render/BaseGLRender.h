//
// Created by DZ0400351 on 2022/8/17.
//

#ifndef MY_APPLICATION_BASEGLRENDER_H
#define MY_APPLICATION_BASEGLRENDER_H


struct TransformMatrix {
    int degree;
    int mirror;
    float translateX;
    float translateY;
    float scaleX;
    float scaleY;
    int angleX;
    int angleY;

    TransformMatrix():
            translateX(0),
            translateY(0),
            scaleX(1.0),
            scaleY(1.0),
            degree(0),
            mirror(0),
            angleX(0),
            angleY(0)
    {

    }
    void Reset()
    {
        translateX = 0;
        translateY = 0;
        scaleX = 1.0;
        scaleY = 1.0;
        degree = 0;
        mirror = 0;

    }
};


class BaseGLRender {
public:
    virtual ~BaseGLRender(){}

    virtual void OnSurfaceCreated(int style) = 0;
    virtual void OnSurfaceChanged(int w, int h) = 0;
    virtual void OnDrawFrame() = 0;

    virtual void UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY) = 0;

    virtual void UpdateMVPMatrix(TransformMatrix * pTransformMatrix) {}

//    virtual void SetTouchLoc(float touchX, float touchY) = 0;
};


#endif //MY_APPLICATION_BASEGLRENDER_H
