//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: video_crt_setup_window.cpp            //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 01.05.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QScreen>
#include <QStyle>

#include "video_crt_setup_window.h"
#include "ui_video_crt_setup_window.h"

VideoCrtSetupWindow::VideoCrtSetupWindow(QWidget *parent, C64Class *c64, VideoCrtClass *video_crt_output, QSettings *ini) :
    QDialog(parent),
    ui(new Ui::VideoCrtSetupWindow),
    isOneShowed(false)
{
    this->ini = ini;
    ui->setupUi(this);

    // Center Window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::screens()[0]->availableGeometry()));

    this->c64 = c64;
    this->video_crt_output = video_crt_output;

    no_video_crt_parameter_updates = true;

    ////////// Load from INI ///////////
    if(ini != nullptr)
    {   
        int value;
        bool bvalue;

        ini->beginGroup("TVSetupWindow");

		value = ini->value("Saettigung",50).toInt();
        if(ui->saettigung_scroll->value() == value) on_saettigung_scroll_valueChanged(value);
        ui->saettigung_scroll->setValue(value);

		value = ini->value("Helligkeit",50).toInt();
        if(ui->helligkeit_scroll->value() == value) on_helligkeit_scroll_valueChanged(value);
        ui->helligkeit_scroll->setValue(value);

        value = ini->value("Kontrast",50).toInt();
        if(ui->kontrast_scroll->value() == value) on_kontrast_scroll_valueChanged(value);
        ui->kontrast_scroll->setValue(value);

        value = ini->value("HorBlurY",2).toInt();
        if(ui->horblurY_scroll->value() == value) on_horblurY_scroll_valueChanged(value);
        ui->horblurY_scroll->setValue(value);

		value = ini->value("HorBlurUV",3).toInt();
        if(ui->horblurUV_scroll->value() == value) on_horblurUV_scroll_valueChanged(value);
        ui->horblurUV_scroll->setValue(value);

		value = ini->value("PhaseLineOffset",1000).toInt();
        if(ui->phase_scroll->value() == value) on_phase_scroll_valueChanged(value);
        ui->phase_scroll->setValue(value);

        value = ini->value("Scanline",75).toInt();
        if(ui->scanline_scroll->value() == value) on_scanline_scroll_valueChanged(value);
        ui->scanline_scroll->setValue(value);

        value = ini->value("Distortion",100).toInt();
        if(ui->distortion_scroll->value() == value) on_distortion_scroll_valueChanged(value);
        ui->distortion_scroll->setValue(value);

        bvalue = ini->value("FirstVICRevision",false).toBool();
        ui->FirstVicRevision->setChecked(bvalue);
        video_crt_output->SetFirstVicRevision(bvalue);

        ini->endGroup();
    }

    no_video_crt_parameter_updates = false;

    ////////////////////////////////////
    video_crt_output->UpdateParameter();
}

VideoCrtSetupWindow::~VideoCrtSetupWindow()
{
    ///////// Save to INI /////////
    if(ini != nullptr)
    {
        ini->beginGroup("TVSetupWindow");
        ini->setValue("Saettigung",ui->saettigung_scroll->value());
        ini->setValue("Helligkeit",ui->helligkeit_scroll->value());
        ini->setValue("Kontrast",ui->kontrast_scroll->value());
        ini->setValue("HorBlurY",ui->horblurY_scroll->value());
        ini->setValue("HorBlurUV",ui->horblurUV_scroll->value());
        ini->setValue("PhaseLineOffset",ui->phase_scroll->value());
        ini->setValue("Scanline",ui->scanline_scroll->value());
        ini->setValue("Distortion",ui->distortion_scroll->value());
        ini->setValue("FirstVICRevision",ui->FirstVicRevision->isChecked());
        ini->endGroup();
    }
    ///////////////////////////////
    delete ui;
}

void VideoCrtSetupWindow::showEvent(QShowEvent*)
{
    isOneShowed = true;
}

void VideoCrtSetupWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void VideoCrtSetupWindow::on_saettigung_scroll_valueChanged(int value)
{
    ui->saettigung_out->setText(QVariant(value).toString());
    video_crt_output->SetSaturation(static_cast<float>(value)/100.0f);
    if(!no_video_crt_parameter_updates) video_crt_output->UpdateParameter();
}

void VideoCrtSetupWindow::on_helligkeit_scroll_valueChanged(int value)
{
    ui->helligkeit_out->setText(QVariant(value).toString());
    video_crt_output->SetBrightness(value/100.0f);
    if(!no_video_crt_parameter_updates) video_crt_output->UpdateParameter();
}

void VideoCrtSetupWindow::on_kontrast_scroll_valueChanged(int value)
{
    ui->kontrast_out->setText(QVariant(value).toString());
    video_crt_output->SetContrast(static_cast<float>(value)/100.0f);
   if(!no_video_crt_parameter_updates) video_crt_output->UpdateParameter();
}

void VideoCrtSetupWindow::on_horblurY_scroll_valueChanged(int value)
{
    ui->horblurY_out->setText(QVariant(value-1).toString() + " Pixel");
    video_crt_output->SetHorizontalBlurY(value);
    if(!no_video_crt_parameter_updates) video_crt_output->UpdateParameter();
}

void VideoCrtSetupWindow::on_horblurUV_scroll_valueChanged(int value)
{
    ui->horblurUV_out->setText(QVariant(value-1).toString() + " Pixel");
    video_crt_output->SetHorizontalBlurUV(value);
    if(!no_video_crt_parameter_updates) video_crt_output->UpdateParameter();
}

void VideoCrtSetupWindow::on_phase_scroll_valueChanged(int value)
{
    float winkel = (value - 1000) * 0.045f;
    ui->phase_out->setText(QVariant(winkel).toString() + tr("°"));
    video_crt_output->SetPhaseAlternatingLineOffset(value);
    if(!no_video_crt_parameter_updates) video_crt_output->UpdateParameter();
}

void VideoCrtSetupWindow::on_scanline_scroll_valueChanged(int value)
{
    ui->scanline_out->setText((QVariant(value).toString() + " %"));
    video_crt_output->SetScanline(value);
    if(!no_video_crt_parameter_updates) video_crt_output->UpdateParameter();
}

void VideoCrtSetupWindow::on_distortion_scroll_valueChanged(int value)
{
    ui->distortion_out->setText(QVariant(value-100).toString());
    c64->SetDistortion((0.01f*(value-100))*MAX_DISTORTION);
}

void VideoCrtSetupWindow::on_FirstVicRevision_clicked(bool checked)
{
    video_crt_output->SetFirstVicRevision(checked);
    if(!no_video_crt_parameter_updates) video_crt_output->UpdateParameter();
}

void VideoCrtSetupWindow::on_Reset_clicked()
{
    ui->saettigung_scroll->setValue(50);
    ui->helligkeit_scroll->setValue(50);
    ui->kontrast_scroll->setValue(50);
    ui->horblurY_scroll->setValue(2);
    ui->horblurUV_scroll->setValue(3);
    ui->phase_scroll->setValue(1000);
	ui->scanline_scroll->setValue(75);
    ui->distortion_scroll->setValue(100);
    ui->FirstVicRevision->setChecked(false);
}


