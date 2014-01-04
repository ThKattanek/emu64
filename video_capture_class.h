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
// Letzte Änderung am 03.01.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VIDEO_CAPTURE_CLASS_H
#define VIDEO_CAPTURE_CLASS_H

// FFMPEG //
#ifndef UINT64_C
#define UINT64_C(c) (c ## ULL)
#endif
extern "C" {
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
}

class VideoCaptureClass
{
public:
    VideoCaptureClass();
    ~VideoCaptureClass();
};

#endif // VIDEO_CAPTURE_CLASS_H
