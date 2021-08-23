//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy1581_window.cpp                 //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 23.08.2021	       		//
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "floppy1581_window.h"
#include "ui_floppy1581_window.h"

Floppy1581Window::Floppy1581Window(QWidget *parent, C64Class *c64) :
	QDialog(parent),
	ui(new Ui::Floppy1581Window)
{
	ui->setupUi(this);

	this->c64 = c64;
}

Floppy1581Window::~Floppy1581Window()
{
	delete ui;
}
