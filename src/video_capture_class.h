//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: video_capture_class.h                 //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 29.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VIDEO_CAPTURE_CLASS_H
#define VIDEO_CAPTURE_CLASS_H

#include <iostream>
#include <SDL2/SDL.h>

using namespace std;

#ifdef _WIN32
    #define PRId64 "I64d"
    #ifndef INT64_C
        #define INT64_C(c) (c ## LL)
        #define UINT64_C(c) (c ## ULL)
    #endif
#endif

extern "C"
{
    #include <libavutil/avassert.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/opt.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/timestamp.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
}

#define STREAM_DURATION   10.0
#define STREAM_FRAME_RATE 50 /* 50 frames/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */
#define SCALE_FLAGS SWS_BICUBIC
#define SOURCE_SAMPLE_BUFFER_LEN 0x100000

typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;
    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;
    AVFrame *frame;
    AVFrame *tmp_frame;
    float t, tincr, tincr2;
    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

class VideoCaptureClass
{
public:
    VideoCaptureClass();
    ~VideoCaptureClass();

    const char *GetAVVersion();
    void SetVideoBitrate(int video_bitrate);
    void SetAudioBitrate(int audio_bitrate);
    bool StartCapture(const char *filename, const char *codec_name, int xw, int yw);
    void StopCapture();
    void SetCapturePause(bool cpt_pause);
    void AddFrame(uint8_t *data, int linesize);
    void FillSourceAudioBuffer(int16_t *data, int len);
    int GetRecordedFrameCount();

    int WriteVideoFrame(AVFormatContext *oc, OutputStream *ost);
    int encode_video;
    AVFormatContext *format_ctx;
    OutputStream video_stream;

    bool mutex_01;

private:
    void AddStream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id);
    void CloseStream(OutputStream *ost);
    bool OpenVideo(AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg);
    AVFrame* AllocPicture(enum AVPixelFormat pix_fmt, int width, int height);
    bool OpenAudio(AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg);
    AVFrame* AllocAudioFrame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);
    int WriteFrame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt);

    int WriteAudioFrame(AVFormatContext *oc, OutputStream *ost);

    // void LogPacket(const AVFormatContext *fmt_ctx, const AVPacket *pkt);

    AVFrame* GetVideoFrame(OutputStream *ost);
    void FillyuvImage(AVFrame *pict, int width, int height);
    AVFrame* GetAudioFrame(OutputStream *ost);

    bool is_capture_cctive;
    bool is_capture_pause;
    int video_xw, video_yw;

    bool have_video, have_audio;
    int encode_audio;

    int video_bitrate, audio_bitrate;


    AVOutputFormat  *output_format;
    OutputStream audio_stream;
    AVCodec *video_codec, *audio_codec;
    AVDictionary *options;

    uint8_t* source_video_data;
    int source_video_line_size;

    unsigned short* source_audio_data;
    int frame_samples_pt;

    int16_t* frame_audio_data_left;
    int16_t* frame_audio_data_right;

    int audio_package_counter;
    int video_package_counter;
};

#endif // VIDEO_CAPTURE_CLASS_H
