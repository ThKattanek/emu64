//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy1581_window.h                   //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 23.08.2021	       		//
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef FLOPPY1581_WINDOW_H
#define FLOPPY1581_WINDOW_H

#include <QDialog>
#include <QTimer>
#include "./c64_class.h"

namespace Ui {
class Floppy1581Window;
}

class Floppy1581Window : public QDialog
{
	Q_OBJECT

public:
	explicit Floppy1581Window(QWidget *parent = nullptr, Floppy1581 *floppy = nullptr);
	~Floppy1581Window();

private slots:
	void onTimer();

private:
	Ui::Floppy1581Window *ui;
	Floppy1581 *floppy;

	QTimer *timer;
};

#endif // FLOPPY1581_WINDOW_H
