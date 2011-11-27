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
// Letzte Änderung am 11.10.2011		//
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
    connect(this,SIGNAL(ChangeGrafikModi(bool,bool,bool,bool)),parent,member);

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

        /// Joyport1
        int index = ui->GamePort1->currentIndex();
        if(index > 1) ini->setValue("JoyPort1",ui->GamePort1->itemText(index));
        else
        {
            if(index == 0) ini->setValue("JoyPort1","none");
                else ini->setValue("JoyPort1","keys");
        }

        /// JoyPort2
        index = ui->GamePort2->currentIndex();
        if(index > 1) ini->setValue("JoyPort2",ui->GamePort2->itemText(index));
        else
        {
            if(index == 0) ini->setValue("JoyPort2","none");
                else ini->setValue("JoyPort2","keys");
        }

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
    on_JoyScan_clicked();
}

void SetupWindow::on_pushButton_clicked()
{

}

void SetupWindow::on_C64Farbmodus_currentIndexChanged(int index)
{
    videopal->SetC64Palette(index);
}

void SetupWindow::on_WPal_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->W32Bit->isChecked());
}

void SetupWindow::on_WDouble_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->W32Bit->isChecked());
}

void SetupWindow::on_W16Bit_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->W32Bit->isChecked());
}

void SetupWindow::on_W32Bit_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->W32Bit->isChecked());
}

void SetupWindow::on_JoyScan_clicked()
{
    // Alle Einträge löschen
    ui->GamePort1->clear();
    ui->GamePort2->clear();

    // Standard Einträge hinzufügen
    ui->GamePort1->addItems(QStringList() << tr("Kein Joystick") << tr("Emulation über Tastatur"));
    ui->GamePort2->addItems(QStringList() << tr("Kein Joystick") << tr("Emulation über Tastatur"));

    // Gefundene Joysticks hinzufügen, haben immer den Index > 1
    if(c64 != 0)
    {
        /// Joystick Update ///
        int JoyAnzahl = c64->GetJoyAnzahl();
        for(int i=0; i < JoyAnzahl; i++)
        {
            ui->GamePort1->addItem(c64->GetJoyName(i));
            ui->GamePort2->addItem(c64->GetJoyName(i));
        }
    }
}

void SetupWindow::on_GamePort1_currentIndexChanged(int index)
{
    /// Wenn der selbe Joystick ausgewählt wird wie Port2 dann Port2 auf "none" setzen
    if((index > 1) && (ui->GamePort2->currentIndex() == index))
    {
        ui->GamePort2->setCurrentIndex(0);
        c64->RemoveJoy(JOY_PORT_2);
    }

    if(index > 1) c64->SetJoy(JOY_PORT_1,index-2);
}

void SetupWindow::on_GamePort2_currentIndexChanged(int index)
{
    /// Wenn der selbe Joystick ausgewählt wird wie Port1 dann Port1 auf "none" setzen
    if((index > 1) && (ui->GamePort1->currentIndex() == index))
    {
        ui->GamePort1->setCurrentIndex(0);
        c64->RemoveJoy(JOY_PORT_1);
    }

    if(index > 1) c64->SetJoy(JOY_PORT_2,index-2);
}
