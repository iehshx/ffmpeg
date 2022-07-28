//
// Created by DZ0400351 on 2022/7/28.
//

#ifndef MY_APPLICATION_DECODER_H
#define MY_APPLICATION_DECODER_H

class Decoder {
public:
    virtual void Start() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual float GetDuration() = 0;
    virtual void SeekToPosition(float position) = 0;
    virtual float GetCurrentPosition() = 0;
};


#endif //MY_APPLICATION_DECODER_H
