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
}

SetupWindow::~SetupWindow()
{
    SaveINI();
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

    ////////// Load from INI ///////////
    if(ini != 0)
    {
        char group_name[32];
        ui->VJoySlots->setRowCount(MAX_VJOYS);
        ui->VJoySlots->setColumnCount(1);
        ui->VJoySlots->setColumnWidth(0,150);

        for(int i=0; i<MAX_VJOYS; i++)
        {
            sprintf(group_name,"VJSlot_%2.2d",i);
            ini->beginGroup(group_name);

            QTableWidgetItem *item = new QTableWidgetItem(ini->value("Name","").toString());
            item->setToolTip(tr("Mit einem doppelklick kann man den Namen ändern."));
            ui->VJoySlots->setItem(i,0,item);
            ui->VJoySlots->setRowHeight(i,18);

            ui->Port1->addItem(QVariant(i).toString() + " " + ini->value("Name","").toString());
            ui->Port2->addItem(QVariant(i).toString() + " " + ini->value("Name","").toString());

            QByteArray array = ini->value("Type",0).toByteArray();
            if (array != 0) for(int j=0;j<5;j++) c64->VJoys[i].Type[j] = array[j];

            array = ini->value("JoyIndex",0).toByteArray();
            if (array != 0) for(int j=0;j<5;j++) c64->VJoys[i].JoyIndex[j] = array[j];

            array = ini->value("KeyDown",0).toByteArray();
            if (array != 0) for(int j=0;j<5;j++) c64->VJoys[i].KeyDown[j] = array[j];

            array = ini->value("KeyUp",0).toByteArray();
            if (array != 0) for(int j=0;j<5;j++) c64->VJoys[i].KeyUp[j] = array[j];

            array = ini->value("ButtonNr",0).toByteArray();
            if (array != 0) for(int j=0;j<5;j++) c64->VJoys[i].ButtonNr[j] = array[j];

            array = ini->value("HatNr",0).toByteArray();
            if (array != 0) for(int j=0;j<5;j++) c64->VJoys[i].HatNr[j] = array[j];

            array = ini->value("HatValue",0).toByteArray();
            if (array != 0) for(int j=0;j<5;j++) c64->VJoys[i].HatValue[j] = array[j];

            array = ini->value("AxisNr",0).toByteArray();
            if (array != 0) for(int j=0;j<5;j++) c64->VJoys[i].AxisNr[j] = array[j];

            array = ini->value("AxisValue",0).toByteArray();
            if (array != 0) for(int j=0;j<5;j++) c64->VJoys[i].AxisValue[j] = array[j];

            ini->endGroup();
        }
        ui->VJoySlots->item(0,0)->setSelected(true);

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

        value = ini->value("Port1",0).toInt();
        ui->Port1->setCurrentIndex(value);
        c64->VPort1 = value;

        value = ini->value("Port2",0).toInt();
        ui->Port2->setCurrentIndex(value);
        c64->VPort2 = value;

        ini->endGroup();
    }
    ////////////////////////////////////
}

void SetupWindow::SaveINI()
{
    if(ini != 0)
    {
        ini->beginGroup("SetupWindow");
        ini->setValue("NoPALColorMode",ui->C64Farbmodus->currentIndex());
        ini->setValue("WindowPalMode",ui->WPal->isChecked());
        ini->setValue("WindowColor32BitMode",ui->W32Bit->isChecked());
        ini->setValue("WindowDoubleSizeMode",ui->WDouble->isChecked());
        ini->setValue("WindowFilter",ui->WFilter->isChecked());
        ini->setValue("Port1",c64->VPort1);
        ini->setValue("Port2",c64->VPort2);
        ini->endGroup();

        char group_name[32];
        for(int i=0; i<MAX_VJOYS; i++)
        {
            sprintf(group_name,"VJSlot_%2.2d",i);
            ini->beginGroup(group_name);
            ini->setValue("Name",c64->VJoys[i].Name);
            ini->setValue("Type",QByteArray((const char*)c64->VJoys[i].Type,5));
            ini->setValue("JoyIndex",QByteArray((const char*)c64->VJoys[i].JoyIndex,5));
            ini->setValue("KeyDown",QByteArray((const char*)c64->VJoys[i].KeyDown,5));
            ini->setValue("KeyUp",QByteArray((const char*)c64->VJoys[i].KeyUp,5));
            ini->setValue("ButtonNr",QByteArray((const char*)c64->VJoys[i].ButtonNr,5));
            ini->setValue("HatNr",QByteArray((const char*)c64->VJoys[i].HatNr,5));
            ini->setValue("HatValue",QByteArray((const char*)c64->VJoys[i].HatValue,5));
            ini->setValue("AxisNr",QByteArray((const char*)c64->VJoys[i].AxisNr,5));
            ini->setValue("AxisValue",QByteArray((const char*)c64->VJoys[i].AxisValue,5));
            ini->endGroup();
        }
    }
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

void SetupWindow::on_ResetSShotCounter_clicked()
{
    emit ResetSreenshotCounter();
}

void SetupWindow::on_RecJoy_clicked()
{
    c64->StartRecJoystickMapping(ui->VJoySlots->selectedItems()[0]->row());
}

void SetupWindow::on_VJoySlots_cellChanged(int row, int column)
{
   strcpy(c64->VJoys[row].Name,ui->VJoySlots->item(row,column)->text().toLatin1().data());

   ui->Port1->setItemText(row,QVariant(row+1).toString() + " " + ui->VJoySlots->item(row,column)->text());
   ui->Port2->setItemText(row,QVariant(row+1).toString() + " " + ui->VJoySlots->item(row,column)->text());
}

void SetupWindow::on_Port1_currentIndexChanged(int index)
{
    c64->VPort1 = index;
}

void SetupWindow::on_Port2_currentIndexChanged(int index)
{
    c64->VPort2 = index;
}
