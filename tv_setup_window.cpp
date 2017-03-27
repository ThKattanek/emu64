//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tv_setup_window.cpp                   //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "tv_setup_window.h"
#include "ui_tv_setup_window.h"

TVSetupWindow::TVSetupWindow(QWidget *parent, C64Class *_c64, VideoPalClass *_videopal, QSettings *_ini) :
    QDialog(parent),
    ui(new Ui::TVSetupWindow),
    isOneShowed(false)
{
    ini = _ini;
    ui->setupUi(this);
    c64 = _c64;
    videopal = _videopal;

    ////////// Load from INI ///////////
    if(ini != 0)
    {
        int value;
        ini->beginGroup("TVSetupWindow");
        if(ini->contains("Geometry")) setGeometry(ini->value("Geometry").toRect());
        value = ini->value("Saettigung",75).toInt();
        ui->saettigung_scroll->setValue(value);
        if(ui->saettigung_scroll->value() == value) on_saettigung_scroll_valueChanged(value);
        value = ini->value("Helligkeit",40).toInt();
        ui->helligkeit_scroll->setValue(value);
        if(ui->helligkeit_scroll->value() == value) on_helligkeit_scroll_valueChanged(value);
        value = ini->value("Kontrast",50).toInt();
        ui->kontrast_scroll->setValue(value);
        if(ui->kontrast_scroll->value() == value) on_kontrast_scroll_valueChanged(value);
        value = ini->value("HorBlurY",2).toInt();
        ui->horblurY_scroll->setValue(value);
        if(ui->horblurY_scroll->value() == value) on_horblurY_scroll_valueChanged(value);
        value = ini->value("HorBlurUV",4).toInt();
        ui->horblurUV_scroll->setValue(value);
        if(ui->horblurUV_scroll->value() == value) on_horblurUV_scroll_valueChanged(value);
        value = ini->value("PhaseLineOffset",840).toInt();
        ui->phase_scroll->setValue(value);
        if(ui->phase_scroll->value() == value) on_phase_scroll_valueChanged(value);
        value = ini->value("Scanline",60).toInt();
        ui->scanline_scroll->setValue(value);
        if(ui->scanline_scroll->value() == value) on_scanline_scroll_valueChanged(value);

        value = ini->value("Distortion",100).toInt();
        ui->distortion_scroll->setValue(value);
        if(ui->distortion_scroll->value() == value) on_distortion_scroll_valueChanged(value);

        ini->endGroup();
    }
    ////////////////////////////////////
    videopal->UpdateParameter();
}

TVSetupWindow::~TVSetupWindow()
{
    ///////// Save to INI /////////
    if(ini != 0)
    {
        ini->beginGroup("TVSetupWindow");
        if(isOneShowed) ini->setValue("Geometry",geometry());
        ini->setValue("Saettigung",ui->saettigung_scroll->value());
        ini->setValue("Helligkeit",ui->helligkeit_scroll->value());
        ini->setValue("Kontrast",ui->kontrast_scroll->value());
        ini->setValue("HorBlurY",ui->horblurY_scroll->value());
        ini->setValue("HorBlurUV",ui->horblurUV_scroll->value());
        ini->setValue("PhaseLineOffset",ui->phase_scroll->value());
        ini->setValue("Scanline",ui->scanline_scroll->value());
        ini->setValue("Distortion",ui->distortion_scroll->value());
        ini->endGroup();
    }
    ///////////////////////////////
    delete ui;
}

void TVSetupWindow::showEvent(QShowEvent*)
{
    isOneShowed = true;
}

void TVSetupWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void TVSetupWindow::on_saettigung_scroll_valueChanged(int value)
{
    ui->saettigung_out->setText(QVariant(value).toString());
    videopal->SetSaturation((float)value/100.0f);
    videopal->UpdateParameter();
}

void TVSetupWindow::on_helligkeit_scroll_valueChanged(int value)
{
    ui->helligkeit_out->setText(QVariant(value).toString());
    videopal->SetHelligkeit((value+50)/100.0f);
    videopal->UpdateParameter();
}

void TVSetupWindow::on_kontrast_scroll_valueChanged(int value)
{
    ui->kontrast_out->setText(QVariant(value).toString());
    videopal->SetKontrast((float)value/100.0f);
    videopal->UpdateParameter();
}

void TVSetupWindow::on_horblurY_scroll_valueChanged(int value)
{
    ui->horblurY_out->setText(QVariant(value-1).toString() + " Pixel");
    videopal->SetHorizontalBlurY(value);
    videopal->UpdateParameter();
}

void TVSetupWindow::on_horblurUV_scroll_valueChanged(int value)
{
    ui->horblurUV_out->setText(QVariant(value-1).toString() + " Pixel");
    videopal->SetHorizontalBlurUV(value);
    videopal->UpdateParameter();
}

void TVSetupWindow::on_phase_scroll_valueChanged(int value)
{
    float winkel = (value - 1000) * 0.045f;
    ui->phase_out->setText(QVariant(winkel).toString() + trUtf8("°"));
    videopal->SetPhaseAltLineOffset(value);
    videopal->UpdateParameter();
}

void TVSetupWindow::on_scanline_scroll_valueChanged(int value)
{
    ui->scanline_out->setText((QVariant(value).toString() + " %"));
    videopal->SetScanline(value);
    videopal->UpdateParameter();
}

void TVSetupWindow::on_distortion_scroll_valueChanged(int value)
{
    ui->distortion_out->setText(QVariant(value-100).toString());
    c64->SetDistortion((0.01f*(value-100))*MAX_DISTORTION);
}

void TVSetupWindow::on_Reset_clicked()
{
    ui->saettigung_scroll->setValue(75);
    ui->helligkeit_scroll->setValue(50);
    ui->kontrast_scroll->setValue(85);
    ui->horblurY_scroll->setValue(2);
    ui->horblurUV_scroll->setValue(4);
    ui->phase_scroll->setValue(840);
    ui->scanline_scroll->setValue(85);
}
