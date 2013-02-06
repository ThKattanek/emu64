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
// Letzte Änderung am 29.07.2012		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include "setup_window.h"
#include "ui_setup_window.h"

SetupWindow::SetupWindow(QWidget *parent,const char *member,VideoPalClass *_videopal, QSettings *_ini) :
    QDialog(parent),
    ui(new Ui::SetupWindow),
    videopal(0),
    c64(0)
{
    videopal = _videopal;
    ini = _ini;
    connect(this,SIGNAL(ChangeGrafikModi(bool,bool,bool,bool,bool)),parent,member);

    ui->setupUi(this);

    ui->C64Farbmodus->addItems(QStringList()<<"Emu64"<<"Emu64 (bis 4.00)"<<"CCS64"<<"PC64"<<"C64S"<<"VICE"<<"FRODO"<<tr("Schwarz / Weiß"));

    ////////// Load from INI ///////////
    if(ini != 0)
    {
        int value;
        bool bvalue;
        ini->beginGroup("SetupWindow");

        value = ini->value("NoPALColorMode",0).toInt();
        ui->C64Farbmodus->setCurrentIndex(value);

        bvalue = ini->value("WindowPalMode",1).toBool();
        ui->WPal->setChecked(bvalue);
        videopal->StartC64isPalmode = bvalue;

        bvalue = ini->value("WindowColor32BitMode",0).toBool();
        ui->W32Bit->setChecked(bvalue);
        videopal->StartC64isColorBit32 = bvalue;

        bvalue = ini->value("WindowDoubleSizeMode",0).toBool();
        ui->WDouble->setChecked(bvalue);
        videopal->StartC64isDoublesize = bvalue;

        bvalue = ini->value("WindowFilter",0).toBool();
        ui->WFilter->setChecked(bvalue);
        on_WFilter_toggled(bvalue);

        ini->endGroup();
    }
    ////////////////////////////////////
}

SetupWindow::~SetupWindow()
{
    ///////// Save to INI /////////
    if(ini != 0)
    {
        ini->beginGroup("SetupWindow");
        ini->setValue("NoPALColorMode",ui->C64Farbmodus->currentIndex());
        ini->setValue("WindowPalMode",ui->WPal->isChecked());
        ini->setValue("WindowColor32BitMode",ui->W32Bit->isChecked());
        ini->setValue("WindowDoubleSizeMode",ui->WDouble->isChecked());
        ini->setValue("WindowFilter",ui->WFilter->isChecked());
        ini->endGroup();
    }
    ///////////////////////////////
    delete ui;
}

void SetupWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void SetupWindow::LoadINI(C64Class *_c64)
{
    c64 = _c64;
}

void SetupWindow::on_C64Farbmodus_currentIndexChanged(int index)
{
    videopal->SetC64Palette(index);
}

void SetupWindow::on_WPal_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->W32Bit->isChecked(),ui->WFilter->isChecked());
}

void SetupWindow::on_WDouble_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->W32Bit->isChecked(),ui->WFilter->isChecked());
}

void SetupWindow::on_W16Bit_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->W32Bit->isChecked(),ui->WFilter->isChecked());
}

void SetupWindow::on_W32Bit_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->W32Bit->isChecked(),ui->WFilter->isChecked());
}

void SetupWindow::on_WFilter_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->W32Bit->isChecked(),ui->WFilter->isChecked());
}

void SetupWindow::ReSetup()
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->W32Bit->isChecked(),ui->WFilter->isChecked());
}

void SetupWindow::on_RecJoy_clicked()
{
    c64->StartRecJoystickMapping(0);
}

void SetupWindow::on_ResetSShotCounter_clicked()
{
    emit ResetSreenshotCounter();
}
