//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy_new_d64_window.h               //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 09.08.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef FLOPPY_NEW_D64_WINDOW_H
#define FLOPPY_NEW_D64_WINDOW_H

#include <QDialog>
#include <QFontDatabase>

namespace Ui {
class FloppyNewD64Window;
}

class FloppyNewD64Window : public QDialog
{
    Q_OBJECT

public:
    explicit FloppyNewD64Window(QWidget *parent = 0);
    ~FloppyNewD64Window();

    QString GetFilename(void);
    QString GetDiskname(void);
    QString GetDiskID(void);

private slots:
    void on_Diskname_textChanged(const QString &arg1);
    void on_DIskID_textChanged(const QString &arg1);

private:
    Ui::FloppyNewD64Window *ui;

    QFont *c64_font1;
};

#endif // FLOPPY_NEW_D64_WINDOW_H
