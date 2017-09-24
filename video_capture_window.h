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
#include <QMessageBox>
#include <QFileDialog>
#include "./c64_class.h"

namespace Ui {
class VideoCaptureWindow;
}

class VideoCaptureWindow : public QDialog
{
    Q_OBJECT

public:
    explicit VideoCaptureWindow(QWidget *parent = 0, C64Class *c64 = 0);
    ~VideoCaptureWindow();
    void RetranslateUi();

private slots:
    void on_CaptureStart_clicked();
    void on_CaptureStop_clicked();
    void on_CapturePause_clicked(bool checked);

private:
    bool getSaveFileName(QWidget *parent, QString caption, QString filter, QString *fileName, QString *fileExt);
    Ui::VideoCaptureWindow *ui;
    C64Class *c64;
};

#endif // VIDEO_CAPTURE_WINDOW_H
