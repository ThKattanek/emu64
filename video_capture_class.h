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
// Letzte Änderung am 11.07.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VIDEO_CAPTURE_CLASS_H
#define VIDEO_CAPTURE_CLASS_H

#include <iostream>
using namespace std;

extern "C"
{
    #define __STDC_CONSTANT_MACROS // for UINT64_C
    #include <libavutil/avutil.h>
    #include <libavutil/version.h>

    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libavformat/avformat.h>
}

class VideoCaptureClass
{
public:
    VideoCaptureClass();
    ~VideoCaptureClass();

    const char *GetAVVersion();
    bool StartCapture(const char *filename, const char *codec_name, int xw, int yw);
    void StopCapture();
    void WriteRGBAFrame(uint8_t *data, int linesize);

private:
    void VideoEncode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,FILE *outfile);

    bool CaptureIsActive;
    int VideoXW, VideoYW;
    AVCodec *VideoCodec;
    AVCodecContext *VideoCodecContext;
    AVPacket *VideoPacket;
    AVFrame *VideoFrame;
    FILE *VideoOutFile;

    uint8_t EndCode[4];
};

#endif // VIDEO_CAPTURE_CLASS_H
