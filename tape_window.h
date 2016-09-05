//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tape_window.h                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 05.09.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef TAPE_WINDOW_H
#define TAPE_WINDOW_H

#include <QDialog>
#include <QSettings>

#include "c64_class.h"

namespace Ui {
class TapeWindow;
}

class TapeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TapeWindow(QWidget *parent = 0, QSettings *_ini = 0, C64Class *c64 = 0);
    ~TapeWindow();
    void RetranslateUi();
    void LoadIni();
    void showEvent(QShowEvent *event);

private:
    Ui::TapeWindow *ui;

    QSettings *ini;
    C64Class *c64;

    bool isOneShowed;
};

#endif // TAPE_WINDOW_H
