//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: floppy_window.h                       //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 18.04.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef FLOPPY_WINDOW_H
#define FLOPPY_WINDOW_H

#include <QDialog>

namespace Ui {
    class FloppyWindow;
}

class FloppyWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FloppyWindow(QWidget *parent = 0);
    ~FloppyWindow();
    void RetranslateUi();

private:
    Ui::FloppyWindow *ui;
};

#endif // FLOPPY_WINDOW_H
