//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: setup_window.cpp                      //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 09.10.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include "setup_window.h"
#include "ui_setup_window.h"

SetupWindow::SetupWindow(QWidget *parent, QSettings *_ini) :
    QDialog(parent),
    ui(new Ui::SetupWindow)
{
    ui->setupUi(this);
    ini = _ini;
}

SetupWindow::~SetupWindow()
{
    delete ui;
}

void SetupWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}
