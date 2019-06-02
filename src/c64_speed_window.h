//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: c64_speed_window.h                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 02.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef C64_SPEED_WINDOW_H
#define C64_SPEED_WINDOW_H

#include <cstdint>
#include <QDialog>
#include <QSettings>
#include "c64_class.h"

namespace Ui {
class C64SpeedWindow;
}

class C64SpeedWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit C64SpeedWindow(QWidget *parent = nullptr, QSettings *ini = nullptr);
    ~C64SpeedWindow();
    void RetranslateUi();
    void SetC64Pointer(C64Class *c64);
    
private slots:
    void showEvent(QShowEvent *event);

    void on_C64Speed_valueChanged(int arg1);
    void on_ScrollC64Speed_valueChanged(int value);
    void on_Set5_clicked();
    void on_Set25_clicked();
    void on_Set50_clicked();
    void on_Set100_clicked();
    void on_Set200_clicked();
    void on_Set300_clicked();
    void on_Set400_clicked();
    void on_Set500_clicked();
    void on_Pause_clicked();
    void on_WarpMode_clicked(bool checked);

    void SetPauseTooltip();

private:
    QSettings *ini;
    Ui::C64SpeedWindow *ui;
    C64Class *c64;
    bool is_one_showed;
    bool pause_status;
};

#endif // C64_SPEED_WINDOW_H
