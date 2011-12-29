//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: debugger_vic_window.h                 //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 29.12.2011		//
// www.emu64.de					//
//						//
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
    void SetC64Pointer(C64Class *_c64);
    void UpdateOutputList(void);

private:
    Ui::DebuggerVicWindow *ui;
    C64Class *c64;
    QStringList GrafikModi;
};

#endif // DEBUGGER_VIC_WINDOW_H
