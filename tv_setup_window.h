//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: tv_setup_window.h                     //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 17.04.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef TV_SETUP_WINDOW_H
#define TV_SETUP_WINDOW_H

#include <QDialog>
#include <QSettings>
#include <qmainwindow.h>
#include "videopal_class.h"

namespace Ui {
    class TVSetupWindow;
}

class TVSetupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TVSetupWindow(QWidget *parent = 0, VideoPalClass *_videopal = 0, QSettings *_ini = 0);
    ~TVSetupWindow();
    void RetranslateUi();

private slots:
    void on_helligkeit_scroll_valueChanged(int value);
    void on_saettigung_scroll_valueChanged(int value);
    void on_kontrast_scroll_valueChanged(int value);
    void on_horblurY_scroll_valueChanged(int value);
    void on_horblurUV_scroll_valueChanged(int value);
    void on_phase_scroll_valueChanged(int value);
    void on_scanline_scroll_valueChanged(int value);

    void on_Reset_clicked();

private:
    void showEvent(QShowEvent* event);

    QSettings *ini;
    Ui::TVSetupWindow *ui;
    VideoPalClass *videopal;
    bool isOneShowed;
};

#endif // TV_SETUP_WINDOW_H
