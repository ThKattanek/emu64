//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: setup_window.cpp                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 20.04.2016                //
// www.emu64.de                                 //
//                                              //
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

    ui->C64Farbmodus->addItems(QStringList()<<"Emu64"<<"Emu64 (bis 4.00)"<<"CCS64"<<"PC64"<<"C64S"<<"VICE"<<"FRODO"<<trUtf8("Schwarz / Weiß"));
}

SetupWindow::~SetupWindow()
{
    SaveINI();
    delete ui;
}

void SetupWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    UpdateToolTips();
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
        ui->VJoySlots->setColumnCount(5);
        ui->VJoySlots->setColumnWidth(0,125);
        ui->VJoySlots->setColumnWidth(1,45);
        ui->VJoySlots->setColumnWidth(2,45);
        ui->VJoySlots->setColumnWidth(3,54);
        ui->VJoySlots->setColumnWidth(4,59);

        QStringList header_label;
        //header_label << trUtf8("Bezeichnung") << "Port 1" << "Port 2" << "" << "";
        //ui->VJoySlots->setHorizontalHeaderLabels(header_label);

        for(int i=0; i<MAX_VJOYS; i++)
        {
            sprintf(group_name,"VJSlot_%2.2d",i);
            ini->beginGroup(group_name);

            QTableWidgetItem *item = new QTableWidgetItem(ini->value("Name","Slot " + QVariant(i+1).toString()).toString());
            //item->setToolTip(trUtf8("Mit einem doppelklick kann man den Namen ändern."));
            ui->VJoySlots->setItem(i,0,item);
            ui->VJoySlots->setRowHeight(i,18);

            item = new QTableWidgetItem("");
            item->setFlags(Qt::ItemIsEditable);
            //item->setToolTip(trUtf8("Verbindet diesen Slot mit dem C64 Gameport 1"));
            ui->VJoySlots->setItem(i,1,item);

            item = new QTableWidgetItem("");
            item->setFlags(Qt::ItemIsEditable);
            //item->setToolTip(trUtf8("Verbindet diesen Slot mit dem C64 Gameport 2"));
            ui->VJoySlots->setItem(i,2,item);

            ButtonMod *button;
            button = new ButtonMod(0,i);
            //button->setText("Lernen");
            connect(button,SIGNAL(Clicked(int,int)),this,SLOT(onClickButton(int,int)));
            ui->VJoySlots->setCellWidget(i,3,button);

            button = new ButtonMod(1,i);
            //button->setText("Löschen");
            connect(button,SIGNAL(Clicked(int,int)),this,SLOT(onClickButton(int,int)));
            ui->VJoySlots->setCellWidget(i,4,button);

            QByteArray array = ini->value("Type",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->VJoys[i].Type[j] = array[j];

            array = ini->value("JoyIndex",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->VJoys[i].JoyIndex[j] = array[j];

            array = ini->value("KeyDown",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->VJoys[i].KeyDown[j] = array[j];

            array = ini->value("KeyUp",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->VJoys[i].KeyUp[j] = array[j];

            array = ini->value("ButtonNr",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->VJoys[i].ButtonNr[j] = array[j];

            array = ini->value("HatNr",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->VJoys[i].HatNr[j] = array[j];

            array = ini->value("HatValue",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->VJoys[i].HatValue[j] = array[j];

            array = ini->value("AxisNr",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->VJoys[i].AxisNr[j] = array[j];

            array = ini->value("AxisValue",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->VJoys[i].AxisValue[j] = array[j];

            ini->endGroup();
        }

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

        bvalue = ini->value("WindowAspectRatio",0).toBool();
        ui->WAspectRatio->setChecked(bvalue);
        c64->SetWindowAspectRatio(bvalue);

        value = ini->value("Port1",0).toInt();
        ui->VJoySlots->item(value,1)->setBackground(QBrush(QColor(50,255,50)));
        ui->VJoySlots->item(value,1)->setText("Port 1");
        c64->VPort1 = value;

        value = ini->value("Port2",0).toInt();
        ui->VJoySlots->item(value,2)->setBackground(QBrush(QColor(50,255,50)));
        ui->VJoySlots->item(value,2)->setText("Port 2");
        c64->VPort2 = value;

        ini->endGroup();


        ini->beginGroup("SetupFullscreen");

        bvalue = ini->value("FullscreenAspectRatio",0).toBool();
        ui->FAspectRatio->setChecked(bvalue);
        c64->SetFullscreenAspectRatio(bvalue);

        ini->endGroup();
    }
    ////////////////////////////////////

    UpdateToolTips();
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
        ini->setValue("WindowAspectRatio",ui->WAspectRatio->isChecked());
        ini->setValue("Port1",c64->VPort1);
        ini->setValue("Port2",c64->VPort2);
        ini->endGroup();

        ini->beginGroup("SetupFullscreen");
        ini->setValue("FullscreenAspectRatio",ui->FAspectRatio->isChecked());
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

void SetupWindow::onClickButton(int idx, int idy)
{
    if(idx == 0)
    {
        /// Lernen ///
        c64->StartRecJoystickMapping(idy);
    }

    if(idx == 1)
    {
        /// Löschen ///
        c64->ClearJoystickMapping(idy);
        ui->VJoySlots->item(idy,0)->setText(c64->VJoys[idy].Name);
    }
}

void SetupWindow::on_VJoySlots_cellChanged(int row, int column)
{
    if(column == 0)
    {
        strcpy(c64->VJoys[row].Name,ui->VJoySlots->item(row,column)->text().toLatin1().data());
    }
}

void SetupWindow::on_VJoySlots_cellClicked(int row, int column)
{
    if(column == 0) return;

    switch(column)
    {
    case 1:
        for(int i=0;i<MAX_VJOYS;i++)
        {
            ui->VJoySlots->item(i,1)->setBackground(QBrush(QColor(255,255,255)));
            ui->VJoySlots->item(i,1)->setText("");
        }
        ui->VJoySlots->item(row,column)->setBackground(QBrush(QColor(50,255,50)));
        ui->VJoySlots->item(row,column)->setText("Port 1");
        c64->VPort1 = row;
        break;

    case 2:
        for(int i=0;i<MAX_VJOYS;i++)
        {
            ui->VJoySlots->item(i,2)->setBackground(QBrush(QColor(255,255,255)));
            ui->VJoySlots->item(i,2)->setText("");
        }
        ui->VJoySlots->item(row,column)->setBackground(QBrush(QColor(50,255,50)));
        ui->VJoySlots->item(row,column)->setText("Port 2");
        c64->VPort2 = row;
        break;
    }
}

void SetupWindow::UpdateToolTips()
{
    QTableWidgetItem *item;
    ButtonMod *button;

    QStringList header_label;
    header_label << trUtf8("Bezeichnung") << "Port 1" << "Port 2" << "" << "";
    ui->VJoySlots->setHorizontalHeaderLabels(header_label);

    for(int i=0; i<MAX_VJOYS; i++)
    {
        item = ui->VJoySlots->item(i,0);
        if(item != NULL) item->setToolTip(trUtf8("Mit einem doppelklick kann man den Namen ändern."));
        item = ui->VJoySlots->item(i,1);
        if(item != NULL) item->setToolTip(trUtf8("Verbindet diesen Slot mit dem C64 Gameport 1"));
        item = ui->VJoySlots->item(i,2);
        if(item != NULL) item->setToolTip(trUtf8("Verbindet diesen Slot mit dem C64 Gameport 2"));


        button = (ButtonMod*)ui->VJoySlots->cellWidget(i,3);
        if(button != NULL) button->setText(trUtf8("Lernen"));
        button = (ButtonMod*)ui->VJoySlots->cellWidget(i,4);
        if(button != NULL) button->setText(trUtf8("Löschen"));
    }
}

void SetupWindow::on_WAspectRatio_clicked(bool checked)
{
    c64->SetWindowAspectRatio(checked);
}

void SetupWindow::on_FAspectRatio_clicked(bool checked)
{
    c64->SetFullscreenAspectRatio(checked);
}
