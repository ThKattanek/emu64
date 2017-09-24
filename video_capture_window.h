//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: video_capture_window.h                //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 24.09.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VIDEO_CAPTURE_WINDOW_H
#define VIDEO_CAPTURE_WINDOW_H

#include <QDialog>

namespace Ui {
class VideoCaptureWindow;
}

class VideoCaptureWindow : public QDialog
{
    Q_OBJECT

public:
    explicit VideoCaptureWindow(QWidget *parent = 0);
    ~VideoCaptureWindow();

private:
    Ui::VideoCaptureWindow *ui;
};

#endif // VIDEO_CAPTURE_WINDOW_H
