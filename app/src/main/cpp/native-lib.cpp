#include <jni.h>
#include <string>

#include "MLOG.h"

extern "C" {
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_getInfo(JNIEnv *env, jobject thiz, jstring url) {
    AVFormatContext *avFormatContext = nullptr;
    AVCodecContext *avCodecContext = nullptr;
    int avFormatContextCode;
    avformat_network_init();
    av_register_all();
    const char *mUrl = env->GetStringUTFChars(url, JNI_FALSE);
    avFormatContextCode = avformat_open_input(&avFormatContext, mUrl, nullptr,
                                              nullptr);
    if (avFormatContextCode != 0) {
        char *err = av_err2str(avFormatContextCode);
        LOGE("avFormatContextCode error = %s", err);
    } else {

        int streamInfoCode = avformat_find_stream_info(avFormatContext, nullptr);
        if (streamInfoCode < 0) {
            char *err = av_err2str(streamInfoCode);
            LOGE("avFormatContextCode error = %s", err);
        } else {
            int avFindBestStreamCode = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_AUDIO, -1,
                                                           -1,
                                                           nullptr, 0);
            if (avFindBestStreamCode >= 0) {
                AVStream *stream = avFormatContext->streams[avFindBestStreamCode];
                AVCodec *avCodec = avcodec_find_decoder(stream->codecpar->codec_id);
                avCodecContext = avcodec_alloc_context3(avCodec);
                avcodec_parameters_to_context(avCodecContext, stream->codecpar);

                int codecCode = avcodec_open2(avCodecContext, avCodec, nullptr);
                if (codecCode != 0) {
                    char *err = av_err2str(codecCode);
                    LOGE("avFormatContextCode error = %s", err);
                }
                LOGE("采样率是:%d,声道数是:%d", avCodecContext->sample_rate, avCodecContext->channels);

                int index = 0;
                while (true) {
                    AVPacket *packet = av_packet_alloc();
                    int ret = av_read_frame(avFormatContext, packet);
                    if (ret < 0) {
                        break;
                    } else {
                        avcodec_send_packet(avCodecContext, packet);
                    }
                    AVFrame *avFrame = av_frame_alloc();
                    int avcodecReceiveFrameCode = avcodec_receive_frame(avCodecContext, avFrame);
                    if (avcodecReceiveFrameCode == 0) {
                        LOGE("解析第%d帧声道数是：%d,,pkt_size :%d", index++, avFrame->channels,
                             avFrame->pkt_size);
                    }

                    av_packet_unref(packet);
                    av_frame_unref(avFrame);
                    av_packet_free(&packet);
                    av_frame_free(&avFrame);
                }


                avcodec_free_context(&avCodecContext);
                avformat_free_context(avFormatContext);
                avformat_network_deinit();
            }
        }
    }


}