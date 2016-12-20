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
// Letzte Änderung am 20.12.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "crt_new_easyflash_window.h"
#include "ui_crt_new_easyflash_window.h"

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
