//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tv_setup_window.h                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 16.04.2023        		//
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef CRT_SETUP_WINDOW_H
#define CRT_SETUP_WINDOW_H

#include <QDialog>
#include <QSettings>
#include <qmainwindow.h>
#include "./c64_class.h"
#include "./video_crt_class.h"

#define MAX_DISTORTION 0.33f

namespace Ui {
    class VideoCrtSetupWindow;
}

class VideoCrtSetupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit VideoCrtSetupWindow(QWidget *parent = nullptr, C64Class *c64 = nullptr, VideoCrtClass *video_crt_output = nullptr, QSettings *ini = nullptr);
    ~VideoCrtSetupWindow();
    void RetranslateUi();
    void ResetAllSettings();

private slots:
    void on_helligkeit_scroll_valueChanged(int value);
    void on_saettigung_scroll_valueChanged(int value);
    void on_kontrast_scroll_valueChanged(int value);
    void on_horblurY_scroll_valueChanged(int value);
    void on_horblurUV_scroll_valueChanged(int value);
    void on_phase_scroll_valueChanged(int value);
    void on_scanline_scroll_valueChanged(int value);
    void on_distortion_scroll_valueChanged(int value);
    void on_FirstVicRevision_clicked(bool checked);
    void on_Reset_clicked();

	void on_enable_user_palette_crt_mode_toggled(bool checked);

    void on_pal_delayline_clicked(bool checked);

    void on_pal_delayline_u_only_clicked(bool checked);

private:
    void showEvent(QShowEvent* event);

    QSettings *ini;
    Ui::VideoCrtSetupWindow *ui;
    C64Class *c64;
    VideoCrtClass *video_crt_output;
    bool isOneShowed;

    bool no_video_crt_parameter_updates;
};

#endif // CRT_SETUP_WINDOW_H
