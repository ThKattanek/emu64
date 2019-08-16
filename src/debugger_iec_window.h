//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: debugger_iec_window.h                 //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 16.08.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef DEBUGGER_IEC_WINDOW_H
#define DEBUGGER_IEC_WINDOW_H

#include <QDialog>
#include <QIcon>

#include "./c64_class.h"

namespace Ui {
class DebuggerIECWindow;
}

class DebuggerIECWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit DebuggerIECWindow(QWidget *parent = nullptr);
    ~DebuggerIECWindow();
    void RetranslateUi();
    void SetC64Pointer(C64Class *c64);
    void UpdateSignals();
    
private slots:
    void on_StopExport_clicked();
    void on_StartExport_clicked();

private:
    Ui::DebuggerIECWindow *ui;

    C64Class *c64;

    QIcon *icon_off;
    QIcon *icon_on;
};

#endif // DEBUGGER_IEC_WINDOW_H
