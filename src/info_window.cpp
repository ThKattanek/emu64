//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: info_window.cpp                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "info_window.h"
#include "ui_info_window.h"

InfoWindow::InfoWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoWindow)
{
    ui->setupUi(this);
}

InfoWindow::~InfoWindow()
{
    delete ui;
}

void InfoWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void InfoWindow::SetMoreInfoText(QString str)
{
    ui->moreinfosOut->setText(str);
}

void InfoWindow::SetEmu64VersionText(QString str)
{
    ui->Emu64Version->setText(str);
}
