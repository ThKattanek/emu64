#include "video_capture_class.h"

VideoCaptureClass::VideoCaptureClass()
{

}

const char* VideoCaptureClass::GetAVVersion()
{
    return av_version_info();
}
