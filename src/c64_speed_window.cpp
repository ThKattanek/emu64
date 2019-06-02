//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: c64_speed_window.cpp                  //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 02.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "c64_speed_window.h"
#include "ui_c64_speed_window.h"

C64SpeedWindow::C64SpeedWindow(QWidget *parent, QSettings *ini) :
    QDialog(parent),
    ini(nullptr),c64(nullptr),pause_status(false)
{
    this->ini = ini;

    ui = new Ui::C64SpeedWindow();
    ui->setupUi(this);
    ui->C64Speed->setValue(100);

    isOneShowed = false;

    ////////// Load from INI ///////////
    if(ini != nullptr)
    {
        ini->beginGroup("C64SpeedWindow");
        if(ini->contains("Geometry")) setGeometry(ini->value("Geometry").toRect());
        else ini->setValue("Geometry", geometry());
        ini->endGroup();
    }
    ////////////////////////////////////
}

C64SpeedWindow::~C64SpeedWindow()
{
    ///////// Save to INI /////////
    if(ini != nullptr)
    {
        ini->beginGroup("C64SpeedWindow");
        if(isOneShowed) ini->setValue("Geometry", geometry());
        ini->endGroup();
    }
    ///////////////////////////////
    delete ui;
}

void C64SpeedWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();

    SetPauseTooltip();
}

void C64SpeedWindow::SetC64Pointer(C64Class *c64)
{
    this->c64 = c64;
}

void C64SpeedWindow::showEvent(QShowEvent*)
{
    isOneShowed = true;
}

void C64SpeedWindow::on_C64Speed_valueChanged(int arg1)
{
    ui->ScrollC64Speed->setValue(arg1);
    if(c64 == nullptr) return;

    c64->SetC64Speed(arg1);
}


void C64SpeedWindow::on_ScrollC64Speed_valueChanged(int value)
{
    ui->C64Speed->setValue(value);
}

void C64SpeedWindow::on_Set5_clicked()
{
    ui->C64Speed->setValue(5);
}

void C64SpeedWindow::on_Set25_clicked()
{
    ui->C64Speed->setValue(25);
}

void C64SpeedWindow::on_Set50_clicked()
{
    ui->C64Speed->setValue(50);
}

void C64SpeedWindow::on_Set100_clicked()
{
    ui->C64Speed->setValue(100);
}

void C64SpeedWindow::on_Set200_clicked()
{
    ui->C64Speed->setValue(200);
}

void C64SpeedWindow::on_Set300_clicked()
{
    ui->C64Speed->setValue(300);
}

void C64SpeedWindow::on_Set400_clicked()
{
    ui->C64Speed->setValue(400);
}

void C64SpeedWindow::on_Set500_clicked()
{
    ui->C64Speed->setValue(500);
}

void C64SpeedWindow::on_Pause_clicked()
{
    if(c64 == nullptr) return;

    if(pause_status)
    {
        pause_status = true;
        ui->Pause->setText(trUtf8("Weiter"));
        c64->SetDebugMode(true);
    }
    else
    {
        pause_status = false;
        ui->Pause->setText(trUtf8("Pause"));
        c64->SetDebugMode(false);
    }
    SetPauseTooltip();
}

void C64SpeedWindow::on_WarpMode_clicked(bool checked)
{
    c64->EnableWarpMode(checked);
}

void C64SpeedWindow::SetPauseTooltip()
{
    if(pause_status)
    {
        ui->Pause->setToolTip(trUtf8("Lässt den C64, mit der eingestellten Geschwindigkeit, weiterlaufen."));
    }
    else
    {
        ui->Pause->setToolTip(trUtf8("Hält den C64 komplett an."));
    }
}
