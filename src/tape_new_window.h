//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tape_new_window.h                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 03.10.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef TAPE_NEW_WINDOW_H
#define TAPE_NEW_WINDOW_H

#include <QDialog>

namespace Ui {
class TapeNewWindow;
}

class TapeNewWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TapeNewWindow(QWidget *parent = 0);
    ~TapeNewWindow();

    QString GetFilename();

private:
    Ui::TapeNewWindow *ui;
};

#endif // TAPE_NEW_WINDOW_H
