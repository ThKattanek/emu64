# //////////////////////////////////////////////////
# //                                              //
# // Emu64                                        //
# // von Thorsten Kattanek                        //
# //                                              //
# // #file: sid_dump_window.h                     //
# //                                              //
# // Dieser Sourcecode ist Copyright geschützt!   //
# // Geistiges Eigentum von Th.Kattanek           //
# //                                              //
# // Letzte Änderung am 11.12.2018                //
# // www.emu64.de                                 //
# //                                              //
# //////////////////////////////////////////////////

#ifndef SID_DUMP_WINDOW_H
#define SID_DUMP_WINDOW_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>
#include <QTimer>
#include "./c64_class.h"

namespace Ui {
class SidDumpWindow;
}

class SidDumpWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SidDumpWindow(QWidget *parent = 0, C64Class *c64 = 0);
    ~SidDumpWindow();
    void RetranslateUi();

private slots:
    bool getSaveFileName(QWidget *parent, QString caption, QString filter, QString *fileName, QString *fileExt);
    void on_SidDumpStart_clicked();
    void on_SidDumpStop_clicked();
    void OnTimer1();

private:
    Ui::SidDumpWindow *ui;
    C64Class *c64;
    QTimer *timer1;
};

#endif // SID_DUMP_WINDOW_H
