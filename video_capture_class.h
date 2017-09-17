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
// Letzte Änderung am 17.09.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VIDEO_CAPTURE_CLASS_H
#define VIDEO_CAPTURE_CLASS_H

#include <iostream>

using namespace std;

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
    void AddFrame(uint8_t *data, int linesize);
    void FillSourceAudioBuffer(uint16_t *data, int len);

private:
    void AddStream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id);
    void CloseStream(AVFormatContext *oc, OutputStream *ost);
    bool OpenVideo(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg);
    AVFrame* AllocPicture(enum AVPixelFormat pix_fmt, int width, int height);
    bool OpenAudio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg);
    AVFrame* AllocAudioFrame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);
    int WriteVideoFrame(AVFormatContext *oc, OutputStream *ost);
    int WriteAudioFrame(AVFormatContext *oc, OutputStream *ost);
    int WriteFrame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt);
    void LogPacket(const AVFormatContext *fmt_ctx, const AVPacket *pkt);

    AVFrame* GetVideoFrame(OutputStream *ost);
    void FillyuvImage(AVFrame *pict, int frame_index, int width, int height);
    AVFrame* GetAudioFrame(OutputStream *ost);

    bool CaptureIsActive;
    int VideoXW, VideoYW;

    bool HaveVideo, HaveAudio;
    int EncodeVideo, EncodeAudio;

    int AudioBitrate, VideoBitrate;

    AVFormatContext *FormatCtx;
    AVOutputFormat  *OutputFormat;
    OutputStream VideoStream, AudioStream;
    AVCodec *AudioCodec, *VideoCodec;
    AVDictionary *Options;

    unsigned char* SourceVideoData;
    int SourceVideoLineSize;

    bool AvailableAudioData;
    unsigned short* SourceAudioData;
    int SourceAudioDataLength;
    int FrameSamplesPt;

    unsigned short* FrameAudioDataL;
    unsigned short* FrameAudioDataR;

};

#endif // VIDEO_CAPTURE_CLASS_H
