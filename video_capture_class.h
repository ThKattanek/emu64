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
// Letzte Änderung am 12.01.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VIDEO_CAPTURE_CLASS_H
#define VIDEO_CAPTURE_CLASS_H

#include <iostream>
using namespace std;

// FFMPEG //
#ifndef UINT64_C
#define UINT64_C(c) (c ## ULL)
#endif
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}

class VideoCaptureClass
{
public:
    VideoCaptureClass();
    ~VideoCaptureClass();
    int Start(char *filename, int video_bit_rate, int xw, int yw, int fps);
    void Stop();
    void AddFrame(void* puffer, int xw, int yw);

private:
    bool    CaptureIsRunning;

    AVCodec *codec;
    AVCodecContext *codec_context;
    AVFrame *picture;
    FILE *file;

    int outbuf_size;
    unsigned char *outbuf;
    int out_size;
    bool AddFrameIsActive;

    unsigned char YUV_COLORS[16*4];
};

#endif // VIDEO_CAPTURE_CLASS_H
