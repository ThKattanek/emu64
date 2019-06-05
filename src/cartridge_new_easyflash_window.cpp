//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: crt_new_easyflash_window.cpp          //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 05.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "cartridge_new_easyflash_window.h"
#include "ui_cartridge_new_easyflash_window.h"

CRTNewEasyflashWindow::CRTNewEasyflashWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CRTNewEasyflashWindow)
{
    ui->setupUi(this);
}

CRTNewEasyflashWindow::~CRTNewEasyflashWindow()
{
    delete ui;
}

QString CRTNewEasyflashWindow::GetFilename()
{
    return ui->Filename->text();
}

QString CRTNewEasyflashWindow::GetCrtName()
{
    return ui->CRTName->text();
}
