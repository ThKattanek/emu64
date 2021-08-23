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

Floppy1581Window::Floppy1581Window(QWidget *parent, Floppy1581 *floppy) :
	QDialog(parent),
	ui(new Ui::Floppy1581Window)
{
	ui->setupUi(this);

	ui->led_green->SetColorOff(QColor(0,20,0));
	ui->led_green->SetColorOn(QColor(10,200,10));

	ui->led_red->SetColorOff(QColor(20,0,0));
	ui->led_red->SetColorOn(QColor(200,10,10));

	this->floppy = floppy;

	timer = new QTimer(this);
	timer->setInterval(10);
	connect(timer,SIGNAL(timeout()),this,SLOT(onTimer()));

	timer->start();
}

Floppy1581Window::~Floppy1581Window()
{
	delete  timer;

	delete ui;
}

void Floppy1581Window::onTimer()
{
	FLOPPY_1581_INFO fi;
	floppy->GetFloppyInfo(&fi);

	if(fi.Power_Led)
		ui->led_red->SetBrightness(1.0f);
	else
		ui->led_red->SetBrightness(0.0f);

	if(fi.Activate_Led)
		ui->led_green->SetBrightness(1.0f);
	else
		ui->led_green->SetBrightness(0.0f);
}
