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
#include "./c64_class.h"

namespace Ui {
class Floppy1581Window;
}

class Floppy1581Window : public QDialog
{
	Q_OBJECT

public:
	explicit Floppy1581Window(QWidget *parent = nullptr, C64Class *c64 = nullptr);
	~Floppy1581Window();

private:
	Ui::Floppy1581Window *ui;
	C64Class *c64;
};

#endif // FLOPPY1581_WINDOW_H
