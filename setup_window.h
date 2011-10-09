//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: setup_window.h                        //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 09.10.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef SETUP_WINDOW_H
#define SETUP_WINDOW_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class SetupWindow;
}

class SetupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SetupWindow(QWidget *parent = 0, QSettings *_ini = 0);
    ~SetupWindow();
    void RetranslateUi();

private:
    Ui::SetupWindow *ui;
    QSettings *ini;
};

#endif // SETUP_WINDOW_H
