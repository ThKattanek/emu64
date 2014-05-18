//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: capture_window.h                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef CAPTURE_WINDOW_H
#define CAPTURE_WINDOW_H

#include <QDialog>
#include <QSettings>
#include <QMessageBox>

#include "c64_class.h"

namespace Ui {
class CaptureWindow;
}

class CaptureWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit CaptureWindow(QWidget *parent = 0, C64Class *c64 = NULL, QSettings *ini = NULL);
    ~CaptureWindow();
    void RetranslateUi();

private slots:
    void on_pushVideoCaptureStart_clicked();

    void on_pushVideoCaptureStop_clicked();

private:
    Ui::CaptureWindow *ui;

    QSettings *ini;
    C64Class *c64;
};

#endif // CAPTURE_WINDOW_H
