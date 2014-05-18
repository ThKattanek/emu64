//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: capture_window.cpp                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "capture_window.h"
#include "ui_capture_window.h"

CaptureWindow::CaptureWindow(QWidget *parent, C64Class *c64, QSettings *ini) :
    QDialog(parent),
    ui(new Ui::CaptureWindow)
{
    this->c64 = c64;
    this->ini = ini;

    ui->setupUi(this);
}

CaptureWindow::~CaptureWindow()
{
    delete ui;
}

void CaptureWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void CaptureWindow::on_pushVideoCaptureStart_clicked()
{
    int ret = c64->StartVideoCapture((char*)"emu64.mpg");
    if(ret != 0)
    {
        QMessageBox::critical(this,"Ein Fehler ist aufgetreten...","-- VideoCaptureClass --\nFehlerCode: " + QVariant(ret).toString());
    }
}

void CaptureWindow::on_pushVideoCaptureStop_clicked()
{
    c64->StopVideoCapture();
}
