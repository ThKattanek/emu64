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
// Letzte Änderung am 28.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QScreen>
#include <QStyle>

#include "./cartridge_new_easyflash_window.h"
#include "./ui_cartridge_new_easyflash_window.h"

CRTNewEasyflashWindow::CRTNewEasyflashWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CRTNewEasyflashWindow)
{
    ui->setupUi(this);

    // Center Window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::screens()[0]->availableGeometry()));
}

CRTNewEasyflashWindow::~CRTNewEasyflashWindow()
{
    delete ui;
}

QString CRTNewEasyflashWindow::GetFilename()
{
    return ui->Filename->text();
}

QString CRTNewEasyflashWindow::GetCartridgeName()
{
    return ui->CRTName->text();
}
