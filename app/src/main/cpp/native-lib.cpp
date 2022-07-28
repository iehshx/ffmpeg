//#include <jni.h>
//#include <string>
//#include <unistd.h>
//#include "pthread.h"
//
//#include "audio_player.h"
//#include "MLOG.h"
//
//extern "C" {
//#include "libavutil/avutil.h"
//#include "libavformat/avformat.h"
//#include "libswresample/swresample.h"
//}
//
//static AudioPlayer *audioPlayer;
//
//void *prepare(void *context) {
//    audioPlayer->init(CHILD);
//    return 0;
//}
//
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_example_myapplication_AudioPlayer_prepareAsync(JNIEnv *env, jobject thiz) {
//    pthread_t pid;
//    pthread_attr_t attr;
//    pthread_attr_init(&attr);
//    pthread_create(&pid, &attr, prepare, nullptr);
//}
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_example_myapplication_AudioPlayer_nPlay(JNIEnv *env, jobject thiz, jstring url) {
//    if (audioPlayer) {//硬件先准备取数据
//        audioPlayer->play();
//    }
//    SwrContext *swrContext = swr_alloc();
//    int avFormatContextCode;
//    avformat_network_init();
//    av_register_all();
//    const char *mUrl = env->GetStringUTFChars(url, JNI_FALSE);
//    avFormatContextCode = avformat_open_input(&avFormatContext, mUrl, nullptr,
//                                              nullptr);
//    if (avFormatContextCode != 0) {
//        char *err = av_err2str(avFormatContextCode);
//        LOGE("avFormatContextCode error = %s", err);
//    } else {
//
//        int streamInfoCode = avformat_find_stream_info(avFormatContext, nullptr);
//        if (streamInfoCode < 0) {
//            char *err = av_err2str(streamInfoCode);
//            LOGE("avFormatContextCode error = %s", err);
//        } else {
//            int avFindBestStreamCode = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_AUDIO, -1,
//                                                           -1,
//                                                           nullptr, 0);
//            if (avFindBestStreamCode >= 0) {
//                AVStream *stream = avFormatContext->streams[avFindBestStreamCode];
//                AVCodec *avCodec = avcodec_find_decoder(stream->codecpar->codec_id);
//                avCodecContext = avcodec_alloc_context3(avCodec);
//                avcodec_parameters_to_context(avCodecContext, stream->codecpar);
//
//                int codecCode = avcodec_open2(avCodecContext, avCodec, nullptr);
//                if (codecCode != 0) {
//                    char *err = av_err2str(codecCode);
//                    LOGE("avFormatContextCode error = %s", err);
//                }
//                LOGE("采样率是:%d,声道数是:%d", avCodecContext->sample_rate, avCodecContext->channels);
//
//                //输入的采样格式
//                enum AVSampleFormat in_sample_fmt = avCodecContext->sample_fmt;
//                //输出采样格式16bit PCM
//                enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
//                //输入采样率
//                int in_sample_rate = avCodecContext->sample_rate;
//                //输出采样率
//                int out_sample_rate = 44100;
////                //输入的声道布局
//                uint64_t in_ch_layout = avCodecContext->channel_layout;
////                //输出的声道布局（立体声）
//                uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
////                //  设置采样参数---------------end
//                swr_alloc_set_opts(swrContext,
//                                   out_ch_layout, out_sample_fmt, out_sample_rate,
//                                   in_ch_layout, in_sample_fmt, in_sample_rate,
//                                   0, NULL);
//                swr_init(swrContext);
//
//
//                if (audioPlayer != NULL) {
//                    audioPlayer->total = avFormatContext->duration / 44100 * 2 * 2;
//                    audioPlayer->time_base = stream->time_base;
//                }
//
//
//                //根据声道布局 获取 输出的声道个数
//                int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
//
//                while (true) {
//                    AVPacket *packet = av_packet_alloc();
//                    int ret = av_read_frame(avFormatContext, packet);
//                    if (ret < 0) {
//                        break;
//                    } else {
//                        avcodec_send_packet(avCodecContext, packet);
//                    }
//                    AVFrame *avFrame = av_frame_alloc();
//                    int avcodecReceiveFrameCode = avcodec_receive_frame(avCodecContext, avFrame);
//                    if (avcodecReceiveFrameCode == 0) {
//                        int out_buffer_size = av_samples_get_buffer_size(NULL, out_channel_nb,
//                                                                         avFrame->nb_samples,
//                                                                         AV_SAMPLE_FMT_S16, 1);
//                        //16bit 44100 PCM 数据 内存空间。
//                        uint8_t *buffer = (uint8_t *) av_malloc(out_buffer_size);
//
//                        swr_convert(swrContext, &buffer, out_buffer_size,
//                                    (const uint8_t **) (avFrame->data),
//                                    avFrame->nb_samples);
//                        audioPlayer->setAudioParams(avCodecContext->sample_rate, out_buffer_size);
//                        //重采样的时候获取当前时间
//                        audioPlayer->current = avFrame->pts / av_q2d(audioPlayer->time_base);
//                        audioPlayer->equeue(buffer);
//
//                        usleep(1000 * 23);
//                    }
//
//                    av_packet_unref(packet);
//                    av_frame_unref(avFrame);
//                    av_packet_free(&packet);
//                    av_frame_free(&avFrame);
//                }
//
//
//                avcodec_free_context(&avCodecContext);
//                avformat_free_context(avFormatContext);
//                avformat_network_deinit();
//            }
//        }
//    }
//}
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_example_myapplication_AudioPlayer_nativeInit(JNIEnv *env, jobject thiz) {
//    audioPlayer = new AudioPlayer(env, thiz);
//}
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_example_myapplication_AudioPlayer_prepare(JNIEnv *env, jobject thiz) {
//    if (audioPlayer) {
//        audioPlayer->init(MAIN);
//    }
//}
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_example_myapplication_AudioPlayer_release(JNIEnv *env, jobject thiz) {
//    if (audioPlayer) {
//        delete audioPlayer;
//    }
//}
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_example_myapplication_AudioPlayer_nStop(JNIEnv *env, jobject thiz) {
//    if (audioPlayer) {
//        audioPlayer->stop();
//    }
//}