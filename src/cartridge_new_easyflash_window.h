//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: crt_new_easyflash_window.h            //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 07.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef CRT_NEW_EASYFLASH_WINDOW_H
#define CRT_NEW_EASYFLASH_WINDOW_H

#include <QDialog>

namespace Ui {
class CRTNewEasyflashWindow;
}

class CRTNewEasyflashWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CRTNewEasyflashWindow(QWidget *parent = nullptr);
    ~CRTNewEasyflashWindow();

    QString GetFilename();
    QString GetCartridgeName();

private:
    Ui::CRTNewEasyflashWindow *ui;
};

#endif // CRT_NEW_EASYFLASH_WINDOW_H
