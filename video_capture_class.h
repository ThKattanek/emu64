#ifndef VIDEO_CAPTURE_CLASS_H
#define VIDEO_CAPTURE_CLASS_H

extern "C"
{
    #define __STDC_CONSTANT_MACROS // for UINT64_C
    #include <libavutil/avutil.h>
    #include <libavutil/version.h>
    #include <libavcodec/avcodec.h>
}

class VideoCaptureClass
{
public:
    VideoCaptureClass();

    const char *GetAVVersion();
};

#endif // VIDEO_CAPTURE_CLASS_H
