//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: video_capture_class.cpp               //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 03.01.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "video_capture_class.h"

VideoCaptureClass::VideoCaptureClass()
{
    avcodec_register_all();
}

VideoCaptureClass::~VideoCaptureClass()
{
}
