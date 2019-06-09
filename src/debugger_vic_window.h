//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: debugger_vic_window.h                 //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 09.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef DEBUGGER_VIC_WINDOW_H
#define DEBUGGER_VIC_WINDOW_H

#include <QDialog>
#include "c64_class.h"

namespace Ui {
    class DebuggerVicWindow;
}

class DebuggerVicWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DebuggerVicWindow(QWidget *parent = 0);
    ~DebuggerVicWindow();
    void RetranslateUi();
    void SetC64Pointer(C64Class *c64);
    void UpdateOutputList();

private:
    Ui::DebuggerVicWindow *ui;
    C64Class *c64;
    QStringList graphic_modi;
};

#endif // DEBUGGER_VIC_WINDOW_H
