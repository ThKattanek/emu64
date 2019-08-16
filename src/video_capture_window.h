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
// Letzte Änderung am 16.08.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VIDEO_CAPTURE_WINDOW_H
#define VIDEO_CAPTURE_WINDOW_H

#include <QDialog>
#include <QTimer>

#include "./c64_class.h"
#include "./custom_save_file_dialog.h"

namespace Ui {
class VideoCaptureWindow;
}

class VideoCaptureWindow : public QDialog
{
    Q_OBJECT

public:
    explicit VideoCaptureWindow(QWidget *parent = nullptr, C64Class *c64 = nullptr);
    ~VideoCaptureWindow();
    void RetranslateUi();

private slots:
    void on_CaptureStart_clicked();
    void on_CaptureStop_clicked();
    void on_CapturePause_clicked(bool checked);
    void OnTimer1();

private:
    Ui::VideoCaptureWindow *ui;
    C64Class *c64;
    QTimer *timer1;
    bool isPause;
    int counter_pause;
};

#endif // VIDEO_CAPTURE_WINDOW_H
