//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: video_capture_window.cpp              //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 24.09.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "video_capture_window.h"
#include "ui_video_capture_window.h"

VideoCaptureWindow::VideoCaptureWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VideoCaptureWindow)
{
    ui->setupUi(this);
}

VideoCaptureWindow::~VideoCaptureWindow()
{
    delete ui;
}
