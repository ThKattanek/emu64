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
// Letzte Änderung am 26.05.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QScreen>
#include <QStyle>

#include "setup_window.h"
#include "ui_setup_window.h"

#define DEFAULT_ROMSET_NAME "Original C64 II"

// Colodore Palette als Default
static const uint8_t default_palette[4 * 16] = {0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x81, 0x33, 0x38, 0x00, 0x75, 0xce, 0xc8, 0x00, 0x8e, 0x3c, 0x97, 0x00,
0x56, 0xac, 0x4d, 0x00, 0x2e, 0x2c, 0x9b, 0x00, 0xed, 0xf1, 0x71, 0x00, 0x8e, 0x50, 0x29, 0x00, 0x55, 0x38, 0x00, 0x00,
0xc4, 0x6c, 0x71, 0x00, 0x4a, 0x4a, 0x4a, 0x00, 0x7b, 0x7b, 0x7b, 0x00, 0x9a, 0xff, 0x9f, 0x00, 0x70, 0x6d, 0xeb, 0x00,
0xb2, 0xb2, 0xb2, 0x00};

SetupWindow::SetupWindow(QWidget *parent, const char *member, VideoCrtClass *video_crt_output, QSettings *ini, QString *romsetPath, QString *dataPath) :
    QDialog(parent),
    ui(new Ui::SetupWindow),
    video_crt_output(nullptr),
    c64(nullptr)
{
    this->video_crt_output = video_crt_output;
    this->ini = ini;
    connect(this,SIGNAL(ChangeGrafikModi(bool,bool,bool,bool)),parent,member);
    this->romsetPath = romsetPath;
    this->dataPath = dataPath;

    ui->setupUi(this);

    // Center Window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::screens()[0]->availableGeometry()));

    // VIC Farbmodi zur ComboBox hinzufügen
    ui->C64Farbmodus->addItems(QStringList()<<"Emu64"<<"Emu64 (bis 4.00)"<<"CCS64"<<"PC64"<<"C64S"<<"Vice"<<"Frodo"<<tr("Pepto")<<tr("Colodore")<<tr("Schwarz / Weiß"));

    // VIC Emulation Default Einstellungen setzen
    ui->VicSpriteViewEnable->setEnabled(true);
    ui->VicBorderViewEnable->setEnabled(true);
    ui->VicSprSprCollisionEnable->setEnabled(true);
    ui->VicSprBgrCollisionEnable->setEnabled(true);
    ui->VicGreyDotsEnable->setEnabled(true);

    // SID Typen zur ComboBox hinzufügen
    ui->FirstSidTyp->addItems(QStringList()<<"MOS-6581"<<"MOS-8580");
    ui->SecondSidTyp->addItems(QStringList()<<"MOS-6581"<<"MOS-8580");

    // SID Default Einstellungen setzen
    ui->SecondSidTyp->setEnabled(false);
    ui->SecondSidAddress->setEnabled(false);
    ui->sid_io_label->setEnabled(false);

    // Second SID Adressbereiche in ComboBox eintragen
    for(int i=0; i<32; i++)
    {
        QString IOAdress = QString().number(i * 32 + 0xD400, 16).prepend("$").toUpper();
        ui->SecondSidAddress->addItem(IOAdress);
    }

    // Screenshotformate in Combobox eintragen
    for(int i=0; i<SCREENSHOT_FORMATS_COUNT; i++)
    {
        ui->ScreenshotFormat->addItem(c64->GetScreenshotFormatName(i));
    }

    FillRomSetCombo();

	// Fullscreen Video Displays and Modes
	int num_displays = c64->GetNumDisplays();

	ui->DisplayList->clear();

	for(int i = 0; i < MAX_VIDEO_DISPLAYS; i++)
		video_display_mode_index[i] = 0;

	for(int i = 0; i < num_displays && i < MAX_VIDEO_DISPLAYS; i++)
	{
		ui->DisplayList->addItem(QString(c64->GetDisplayName(i)) + "  [" + QString::number(i,10) + "]");
	}

	is_filled_display_mode_list = false;
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

void SetupWindow::LoadINI(C64Class *c64)
{
    this->c64 = c64;

    ////////// Load from INI ///////////
    if(ini != 0)
    {
        char group_name[32];
        ui->VJoySlots->setRowCount(MAX_VJOY_NUM);
        ui->VJoySlots->setColumnCount(5);
		ui->VJoySlots->setColumnWidth(0,130);
		ui->VJoySlots->setColumnWidth(1,50);
		ui->VJoySlots->setColumnWidth(2,50);
		ui->VJoySlots->setColumnWidth(3,90);
		ui->VJoySlots->setColumnWidth(4,90);

        QStringList header_label;
        //header_label << tr("Bezeichnung") << "Port 1" << "Port 2" << "" << "";
        //ui->VJoySlots->setHorizontalHeaderLabels(header_label);

        for(int i=0; i<MAX_VJOY_NUM; i++)
        {
            sprintf(group_name,"VJSlot_%2.2d",i);
            ini->beginGroup(group_name);

            QTableWidgetItem *item = new QTableWidgetItem(ini->value("Name","Slot " + QVariant(i+1).toString()).toString());
            item->setToolTip(tr("Mit einem doppelklick kann man den Namen ändern."));
            ui->VJoySlots->setItem(i,0,item);
            ui->VJoySlots->setRowHeight(i,18);

            item = new QTableWidgetItem("");
            item->setFlags(Qt::ItemIsEditable);
            item->setToolTip(tr("Verbindet diesen Slot mit dem C64 Gameport 1"));
            ui->VJoySlots->setItem(i,1,item);

            item = new QTableWidgetItem("");
            item->setFlags(Qt::ItemIsEditable);
            item->setToolTip(tr("Verbindet diesen Slot mit dem C64 Gameport 2"));
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
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->virtual_joys[i].Type[j] = array[j];

            array = ini->value("JoyIndex",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->virtual_joys[i].JoyIndex[j] = array[j];

            array = ini->value("KeyDown",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->virtual_joys[i].KeyDown[j] = array[j];

            array = ini->value("KeyUp",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->virtual_joys[i].KeyUp[j] = array[j];

            array = ini->value("ButtonNr",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->virtual_joys[i].ButtonNr[j] = array[j];

            array = ini->value("HatNr",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->virtual_joys[i].HatNr[j] = array[j];

            array = ini->value("HatValue",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->virtual_joys[i].HatValue[j] = array[j];

            array = ini->value("AxisNr",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->virtual_joys[i].AxisNr[j] = array[j];

            array = ini->value("AxisValue",0).toByteArray();
            if (!array.isEmpty()) for(int j=0;j<5;j++) c64->virtual_joys[i].AxisValue[j] = array[j];

            ini->endGroup();
        }

        int value;
        int value1;
        int value2;
        bool bvalue;
        QString svalue;

        ini->beginGroup("SetupWindow");

        value = ini->value("NoPALColorMode",7).toInt();
        ui->C64Farbmodus->setCurrentIndex(value);

		bvalue = ini->value("WindowPalMode",1).toBool();
        ui->WPal->setChecked(bvalue);
        video_crt_output->start_c64_is_palmode = bvalue;

		bvalue = ini->value("WindowDoubleSizeMode",1).toBool();
        ui->WDouble->setChecked(bvalue);
        video_crt_output->start_c64_is_doublesize = bvalue;

		bvalue = ini->value("WindowFilter",1).toBool();
        ui->WFilter->setChecked(bvalue);

		bvalue = ini->value("WindowAspectRatio",0).toBool();
        ui->WAspectRatio->setChecked(bvalue);
        c64->SetWindowAspectRatio(bvalue);

        value = ini->value("Port1",0).toInt();
        ui->VJoySlots->item(value,1)->setBackground(QBrush(QColor(50,255,50)));
        ui->VJoySlots->item(value,1)->setText("Port 1");
        c64->virtual_port1 = value;

        value = ini->value("Port2",0).toInt();
        ui->VJoySlots->item(value,2)->setBackground(QBrush(QColor(50,255,50)));
        ui->VJoySlots->item(value,2)->setText("Port 2");
        c64->virtual_port2 = value;

        value = ini->value("MousePort",0).toInt();
        ui->MausPort->setCurrentIndex(value);
        c64->SetMouse1351Port(value);

		bvalue = ini->value("AutoMouseHide",true).toBool();
        value = ini->value("AutoMouseHideTime",3).toInt();

        if(!bvalue)
        {
            ui->AutoMouseHide->setChecked(false);
            ui->AutoMouseHideTime->setEnabled(false);
        }
        else
        {
            ui->AutoMouseHide->setChecked(true);
            ui->AutoMouseHideTime->setEnabled(true);
        }

        ui->AutoMouseHideTime->setValue(value);

        svalue = ini->value("RomSet",DEFAULT_ROMSET_NAME).toString();
        value = ui->SelectRomSet->findText(svalue);
        if(value != -1) ui->SelectRomSet->setCurrentIndex(value);

        ini->endGroup();

        ini->beginGroup("SetupFullscreen");

        bvalue = ini->value("FullscreenAspectRatio",true).toBool();
        ui->FAspectRatio->setChecked(bvalue);
        c64->SetFullscreenAspectRatio(bvalue);

        ini->endGroup();

        ini->beginGroup("Sound");

        value = ini->value("SIDVolume",100).toInt();
        ui->SIDVolume->setValue(value);
        c64->SetSIDVolume(value / 100.0);

        ini->endGroup();

        ini->beginGroup("SID");
        svalue = ini->value("SID1Typ","8580").toString();

        if(svalue == "8580")
        {
            ui->FirstSidTyp->setCurrentIndex(1);
            c64->SetFirstSidTyp(MOS_8580);
        }
        if(svalue == "6581")
        {
            ui->FirstSidTyp->setCurrentIndex(0);
            c64->SetFirstSidTyp(MOS_6581);
        }

        svalue = ini->value("SID2Typ","8580").toString();
        if(svalue == "8580")
        {
            ui->SecondSidTyp->setCurrentIndex(1);
            c64->SetSecondSidTyp(MOS_8580);
        }
        if(svalue == "6581")
        {
            ui->SecondSidTyp->setCurrentIndex(0);
            c64->SetSecondSidTyp(MOS_6581);
        }

        bvalue = ini->value("StereoSid",false).toBool();
        ui->SecondSidEnable->setChecked(bvalue);
        on_SecondSidEnable_toggled(bvalue);

        value = ini->value("StereoSidAddress",1).toInt();
        ui->SecondSidAddress->setCurrentIndex(value);
        on_SecondSidAddress_currentIndexChanged(value);

        bvalue = ini->value("6ChannelMode",false).toBool();
        ui->Sid6ChannelMode->setChecked(bvalue);
        on_Sid6ChannelMode_toggled(bvalue);

        bvalue = ini->value("CycleExact",true).toBool();
        ui->SidCycleExactEnable->setChecked(bvalue);
        on_SidCycleExactEnable_toggled(bvalue);

        bvalue = ini->value("Filter",true).toBool();
        ui->SidFilterEnable->setChecked(bvalue);
        on_SidFilterEnable_toggled(bvalue);

        ini->endGroup();

        ini->beginGroup("VIC");

        bvalue = ini->value("SpriteView",true).toBool();
        ui->VicSpriteViewEnable->setChecked(bvalue);
        on_VicSpriteViewEnable_toggled(bvalue);

        bvalue = ini->value("BorderView",true).toBool();
        ui->VicBorderViewEnable->setChecked(bvalue);
        on_VicBorderViewEnable_toggled(bvalue);

        bvalue = ini->value("SpriteSpriteCollision",true).toBool();
        ui->VicSprSprCollisionEnable->setChecked(bvalue);
        on_VicSprSprCollisionEnable_toggled(bvalue);

        bvalue = ini->value("SpriteBackgroundCollision",true).toBool();
        ui->VicSprBgrCollisionEnable->setChecked(bvalue);
        on_VicSprBgrCollisionEnable_toggled(bvalue);

        bvalue = ini->value("GreyDots",false).toBool();
        ui->VicGreyDotsEnable->setChecked(bvalue);
        on_VicGreyDotsEnable_toggled(bvalue);

        //FirstLine PAL: VICE - 16, Emu64 - 26
        //LastLine PAL:  VICE - 288, Emu64 - 292
        value1 = ini->value("FirstDisplayLinePAL",16).toInt();
        value2 = ini->value("LastDisplayLinePAL",288).toInt();
        c64->SetVicDisplaySizePal(value1, value2);

        value1 = ini->value("FirstDisplayLineNTSC",30).toInt();
        value2 = ini->value("LastDisplayLineNTSC",288).toInt();
        c64->SetVicDisplaySizeNtsc(value1, value2);

		bvalue = ini->value("VSync",false).toBool();
		ui->Vsync->setChecked(bvalue);
		on_Vsync_clicked(bvalue);


		char display_name[64];
		for(int i=0; i<MAX_VIDEO_DISPLAYS; i++)
		{
			sprintf(display_name, "VideoDisplay_%d_FullscreenVideoMode",i);
			video_display_mode_index[i] = ini->value(display_name,0).toInt();
			c64->SetFullscreenDisplayMode(i,video_display_mode_index[i]);
		}

		ui->VideoModes->setCurrentIndex(video_display_mode_index[0]);

        ini->endGroup();

        ini->beginGroup("MainWindow");
        value = ini->value("ScreenshotFormatNr",SCREENSHOT_FORMAT_PNG).toInt();
        ui->ScreenshotFormat->setCurrentIndex(value);
        ini->endGroup();

        ini->beginGroup("Testarea");
        value1 = ini->value("C64CyclesPerSecond",985248).toInt();
        ui->cycles_per_second->setValue(value1);
        c64->SetC64Frequency(value1);
        ini->endGroup();

		ini->beginGroup("C64UserPalette");
		bvalue = ini->value("EnableUserPalette", false).toBool();
		video_crt_output->EnableUserPalette(bvalue);
		ui->EnableUserPalette->setChecked(bvalue);
		ini->endGroup();

		char key_name[32];
		QColor default_color = Qt::white;

		ini->beginGroup("C64UserPalette");
		for(int i=0; i<16; i++)
		{
			sprintf(key_name, "Color_%2.2d", i);

			default_color.setRed(default_palette[i*4+0]);
			default_color.setGreen(default_palette[i*4+1]);
			default_color.setBlue(default_palette[i*4+2]);

			QColor color = ini->value(key_name, default_color.rgba()).toUInt();
			video_crt_output->SetUserPaletteColor(i, color.red(), color.green(), color.blue());
		}
		ini->endGroup();
    }
    ////////////////////////////////////

    UpdateToolTips();
}

void SetupWindow::SaveINI()
{
    if(ini != nullptr)
    {
        ini->beginGroup("SetupWindow");
        ini->setValue("NoPALColorMode",ui->C64Farbmodus->currentIndex());
        ini->setValue("WindowPalMode",ui->WPal->isChecked());
        ini->setValue("WindowDoubleSizeMode",ui->WDouble->isChecked());
        ini->setValue("WindowFilter",ui->WFilter->isChecked());
        ini->setValue("WindowAspectRatio",ui->WAspectRatio->isChecked());
        ini->setValue("Port1",c64->virtual_port1);
        ini->setValue("Port2",c64->virtual_port2);
        ini->setValue("MousePort",ui->MausPort->currentIndex());
        ini->setValue("AutoMouseHide",ui->AutoMouseHide->isChecked());
        ini->setValue("AutoMouseHideTime",ui->AutoMouseHideTime->value());
        ini->setValue("RomSet",ui->SelectRomSet->currentText());
        ini->endGroup();

        ini->beginGroup("SetupFullscreen");
        ini->setValue("FullscreenAspectRatio",ui->FAspectRatio->isChecked());
        ini->endGroup();

        char group_name[32];
        for(int i=0; i<MAX_VJOY_NUM; i++)
        {
            sprintf(group_name,"VJSlot_%2.2d",i);
            ini->beginGroup(group_name);
            ini->setValue("Name",c64->virtual_joys[i].Name);
            ini->setValue("Type",QByteArray((const char*)c64->virtual_joys[i].Type,5));
            ini->setValue("JoyIndex",QByteArray((const char*)c64->virtual_joys[i].JoyIndex,5));
            ini->setValue("KeyDown",QByteArray((const char*)c64->virtual_joys[i].KeyDown,5));
            ini->setValue("KeyUp",QByteArray((const char*)c64->virtual_joys[i].KeyUp,5));
            ini->setValue("ButtonNr",QByteArray((const char*)c64->virtual_joys[i].ButtonNr,5));
            ini->setValue("HatNr",QByteArray((const char*)c64->virtual_joys[i].HatNr,5));
            ini->setValue("HatValue",QByteArray((const char*)c64->virtual_joys[i].HatValue,5));
            ini->setValue("AxisNr",QByteArray((const char*)c64->virtual_joys[i].AxisNr,5));
            ini->setValue("AxisValue",QByteArray((const char*)c64->virtual_joys[i].AxisValue,5));
            ini->endGroup();
        }

        ini->beginGroup("Sound");
        ini->setValue("SIDVolume",ui->SIDVolume->value());
        ini->endGroup();

        ini->beginGroup("SID");

        switch (ui->FirstSidTyp->currentIndex()) {
        case MOS_8580:
            ini->setValue("SID1Typ","8580");
            break;

        case MOS_6581:
            ini->setValue("SID1Typ","6581");
            break;
        default:
            break;
        }

        switch (ui->SecondSidTyp->currentIndex()) {
        case MOS_8580:
            ini->setValue("SID2Typ","8580");
            break;

        case MOS_6581:
            ini->setValue("SID2Typ","6581");
            break;
        default:
            break;
        }

        ini->setValue("StereoSid",ui->SecondSidEnable->isChecked());
        ini->setValue("StereoSidAddress",ui->SecondSidAddress->currentIndex());
        ini->setValue("6ChannelMode",ui->Sid6ChannelMode->isChecked());
        ini->setValue("CycleExact",ui->SidCycleExactEnable->isChecked());
        ini->setValue("Filter",ui->SidFilterEnable->isChecked());

        ini->endGroup();

        ini->beginGroup("VIC");
        ini->setValue("SpriteView",ui->VicSpriteViewEnable->isChecked());
        ini->setValue("BorderView",ui->VicBorderViewEnable->isChecked());
        ini->setValue("SpriteSpriteCollision",ui->VicSprSprCollisionEnable->isChecked());
        ini->setValue("SpriteBackgroundCollision",ui->VicSprBgrCollisionEnable->isChecked());
        ini->setValue("GreyDots",ui->VicGreyDotsEnable->isChecked());

        ini->setValue("FirstDisplayLinePAL",c64->GetVicFirstDisplayLinePal());
        ini->setValue("LastDisplayLinePAL",c64->GetVicLastDisplayLinePal());
        ini->setValue("FirstDisplayLineNTSC",c64->GetVicFirstDisplayLineNtsc());
        ini->setValue("LastDisplayLineNTSC",c64->GetVicLastDisplayLineNtsc());

		ini->setValue("VSync",ui->Vsync->isChecked());

		char display_name[64];
		for(int i=0; i<MAX_VIDEO_DISPLAYS; i++)
		{
			sprintf(display_name, "VideoDisplay_%d_FullscreenVideoMode",i);
			ini->setValue(QString(display_name), video_display_mode_index[i]);
		}

        ini->endGroup();

        ini->beginGroup("MainWindow");
        ini->setValue("ScreenshotFormatNr",ui->ScreenshotFormat->currentIndex());
        ini->endGroup();

        ini->beginGroup("Testarea");
        ini->setValue("C64CyclesPerSecond",ui->cycles_per_second->value());
        ini->endGroup();

		ini->beginGroup("C64UserPalette");
		ini->setValue("EnableUserPalette", ui->EnableUserPalette->isChecked());

		ini->endGroup();
    }
}

void SetupWindow::on_C64Farbmodus_currentIndexChanged(int index)
{
    video_crt_output->SetC64Palette(index);
}

void SetupWindow::on_WPal_toggled(bool checked)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->WFilter->isChecked());
	if(checked)
	{
		ui->EnableUserPalette->setEnabled(false);
		ui->C64Farbmodus->setEnabled(false);
	}
	else
	{
		ui->EnableUserPalette->setEnabled(true);
		ui->C64Farbmodus->setEnabled(true);
	}
}

void SetupWindow::on_WDouble_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->WFilter->isChecked());
}

void SetupWindow::on_WFilter_toggled(bool)
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->WFilter->isChecked());
}

void SetupWindow::ReSetup()
{
    emit ChangeGrafikModi(false,ui->WPal->isChecked(),ui->WDouble->isChecked(),ui->WFilter->isChecked());
}

int SetupWindow::GetScreenshotFormat()
{
    return ui->ScreenshotFormat->currentIndex();
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
        c64->SetFocusToC64Window();
        c64->StartRecJoystickMapping(idy);
    }

    if(idx == 1)
    {
        /// Löschen ///
        c64->ClearJoystickMapping(idy);
        ui->VJoySlots->item(idy,0)->setText(c64->virtual_joys[idy].Name);
    }
}

void SetupWindow::on_VJoySlots_cellChanged(int row, int column)
{
    if(column == 0)
    {
        strcpy(c64->virtual_joys[row].Name,ui->VJoySlots->item(row,column)->text().toLocal8Bit());
    }
}

void SetupWindow::on_VJoySlots_cellClicked(int row, int column)
{
    if(column == 0) return;

    switch(column)
    {
    case 1:
        for(int i=0;i<MAX_VJOY_NUM;i++)
        {
            ui->VJoySlots->item(i,1)->setBackground(QBrush(QColor(255,255,255)));
            ui->VJoySlots->item(i,1)->setText("");
        }
        ui->VJoySlots->item(row,column)->setBackground(QBrush(QColor(50,255,50)));
        ui->VJoySlots->item(row,column)->setText("Port 1");
        c64->virtual_port1 = row;
        break;

    case 2:
        for(int i=0;i<MAX_VJOY_NUM;i++)
        {
            ui->VJoySlots->item(i,2)->setBackground(QBrush(QColor(255,255,255)));
            ui->VJoySlots->item(i,2)->setText("");
        }
        ui->VJoySlots->item(row,column)->setBackground(QBrush(QColor(50,255,50)));
        ui->VJoySlots->item(row,column)->setText("Port 2");
        c64->virtual_port2 = row;
        break;
    }
}

void SetupWindow::UpdateToolTips()
{
    QTableWidgetItem *item;
    ButtonMod *button;

    QStringList header_label;
    header_label << tr("Bezeichnung") << "Port 1" << "Port 2" << "" << "";
    ui->VJoySlots->setHorizontalHeaderLabels(header_label);

    for(int i=0; i<MAX_VJOY_NUM; i++)
    {
        item = ui->VJoySlots->item(i,0);
        if(item != NULL) item->setToolTip(tr("Mit einem doppelklick kann man den Namen ändern."));
        item = ui->VJoySlots->item(i,1);
        if(item != NULL) item->setToolTip(tr("Verbindet diesen Slot mit dem C64 Gameport 1"));
        item = ui->VJoySlots->item(i,2);
        if(item != NULL) item->setToolTip(tr("Verbindet diesen Slot mit dem C64 Gameport 2"));


        button = (ButtonMod*)ui->VJoySlots->cellWidget(i,3);
        if(button != NULL) button->setText(tr("Lernen"));
        button = (ButtonMod*)ui->VJoySlots->cellWidget(i,4);
        if(button != NULL) button->setText(tr("Löschen"));
    }
}

void SetupWindow::FillRomSetCombo()
{
    // Alle RomSets auslesen und in ComboBox eintragen

    ui->SelectRomSet->clear();
    ui->SelectRomSet->addItem(DEFAULT_ROMSET_NAME);

    QStringList romset_names = GetAllRomsetNames(romsetPath);
    ui->SelectRomSet->addItems(romset_names);
}

QStringList SetupWindow::GetAllRomsetNames(const QString *romset_dir)
{
    QStringList romset_names;
    if(romset_dir == NULL) return romset_names;

    QDir dir(*romset_dir);

    if(dir.exists())
    {
        QDirIterator directories(*romset_dir, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

        while(directories.hasNext())
        {
            directories.next();

            // Prüfen ob alle RomFiles vorhanden sind
            QFile file;
            int anz = 0;

            // Auf Kernal Rom prüfen
            file.setFileName(directories.filePath() + "/kernal.rom");
            if(file.exists())
                if(file.size() == 8192)
                    anz++;

            // Auf Basic Rom prüfen
            file.setFileName(directories.filePath() + "/basic.rom");
            if(file.exists())
                if(file.size() == 8192)
                    anz++;

            // Auf Charakter Rom prüfen
            file.setFileName(directories.filePath() + "/char.rom");
            if(file.exists())
                if(file.size() == 4096)
                    anz++;

            // Auf 1541-Dos Rom prüfen (Floppy 0)
            file.setFileName(directories.filePath() + "/1541.rom");
            if(file.exists())
                if(file.size() == 16384)
                    anz++;

            if(anz == 4)
            {
                // Es handelt sich um ein gültiges RomSet
                romset_names << directories.fileName();
            }
        }
    }

    return romset_names;
}

void SetupWindow::on_WAspectRatio_clicked(bool checked)
{
    c64->SetWindowAspectRatio(checked);
}

void SetupWindow::on_FAspectRatio_clicked(bool checked)
{
    c64->SetFullscreenAspectRatio(checked);
}

void SetupWindow::on_MausPort_currentIndexChanged(int index)
{
    c64->SetMouse1351Port(index);
}

void SetupWindow::on_SIDVolume_valueChanged(int value)
{
   ui->SIDVolumeOut->setText(QVariant(value).toString() + "%");
   c64->SetSIDVolume(value / 100.0);
}

void SetupWindow::on_AutoMouseHide_clicked(bool checked)
{
    if(checked)
    {
        ui->AutoMouseHideTime->setEnabled(true);
        c64->SetMouseHiddenTime(ui->AutoMouseHideTime->value() * 1000);
    }
    else
    {
        ui->AutoMouseHideTime->setEnabled(false);
        c64->SetMouseHiddenTime(0);
    }
}

void SetupWindow::on_AutoMouseHideTime_valueChanged(int arg1)
{
    if(ui->AutoMouseHide->isChecked()) c64->SetMouseHiddenTime(arg1 * 1000);
    else c64->SetMouseHiddenTime(0);
}

void SetupWindow::on_FirstSidTyp_currentIndexChanged(int index)
{
    if (index > 1) return;
    if (c64 == NULL) return;

    c64->SetFirstSidTyp(index);
}

void SetupWindow::on_SecondSidTyp_currentIndexChanged(int index)
{
    if (index > 1) return;
    if (c64 == NULL) return;

    c64->SetSecondSidTyp(index);
}

void SetupWindow::on_SecondSidAddress_currentIndexChanged(int index)
{
    if(c64 == NULL) return;

    unsigned short sid_address = index * 0x20 + 0xd400;
    c64->SetStereoSidAddress(sid_address);
}

void SetupWindow::on_SecondSidEnable_toggled(bool checked)
{
    if(c64 != NULL)
        c64->EnableStereoSid(checked);

    ui->SecondSidTyp->setEnabled(checked);
    ui->SecondSidAddress->setEnabled(checked);
    ui->sid_io_label->setEnabled(checked);
    ui->Sid6ChannelMode->setEnabled(checked);

    if(!checked)
    {
        ui->sid1_label->setText("1. SID [L/R]");
        ui->SecondSidEnable->setText("2. SID");
    }
    else
    {
        if(ui->Sid6ChannelMode->isChecked())
        {
            ui->sid1_label->setText("1. SID [L/R]");
            ui->SecondSidEnable->setText("2. SID [L/R]");
        }
        else
        {
            ui->sid1_label->setText("1. SID [L]");
            ui->SecondSidEnable->setText("2. SID [R]");
        }
    }
}

void SetupWindow::on_SidCycleExactEnable_toggled(bool checked)
{
    if(c64 != NULL)
        c64->SetSidCycleExact(checked);
}

void SetupWindow::on_SidFilterEnable_toggled(bool checked)
{
    if(c64 != NULL)
        c64->SetSidFilter(checked);
}

void SetupWindow::on_Sid6ChannelMode_toggled(bool checked)
{
    if(c64 != NULL)
        c64->SetStereoSid6ChannelMode(checked);

    if(!ui->SecondSidEnable->isChecked()) return;

    if(checked)
    {
        ui->sid1_label->setText("1. SID [L/R]");
        ui->SecondSidEnable->setText("2. SID [L/R]");
    }
    else
    {
        ui->sid1_label->setText("1. SID [L]");
        ui->SecondSidEnable->setText("2. SID [R]");
    }
}

void SetupWindow::on_SelectRomSet_currentIndexChanged(const QString &arg1)
{
    QString kernal_rom, basic_rom, char_rom, dos1541_rom;

    if(arg1 == DEFAULT_ROMSET_NAME)
    {
        // Default RomSet laden
        kernal_rom = *dataPath+"/roms/kernal.rom";
        basic_rom = *dataPath+"/roms/basic.rom";
        char_rom = *dataPath+"/roms/char.rom";
        dos1541_rom = *dataPath+"/roms/1541.rom";

        ui->DeleteRomSet->setEnabled(false);
    }
    else
    {
        kernal_rom = *romsetPath + arg1 + "/kernal.rom";
        basic_rom = *romsetPath + arg1 + "/basic.rom";
        char_rom = *romsetPath + arg1 + "/char.rom";
        dos1541_rom = *romsetPath + arg1 + "/1541.rom";

        ui->DeleteRomSet->setEnabled(true);
    }

    if(c64 != NULL)
    {
        c64->LoadC64Roms(kernal_rom.toLocal8Bit(),basic_rom.toLocal8Bit(),char_rom.toLocal8Bit());

        for(int i=0; i<MAX_FLOPPY_NUM; i++)
        {
            c64->LoadFloppyRom(i,dos1541_rom.toLocal8Bit());
        }

        c64->HardReset();
    }
}

void SetupWindow::on_DeleteRomSet_clicked()
{
    if(QMessageBox::Yes == QMessageBox::question(this,tr("Löschen?"),tr("Möchten Sie dieses RomSet wirklich entfernen?\n\n") + "[" + ui->SelectRomSet->currentText() + "]",QMessageBox::Yes | QMessageBox::No))
    {
        if(!RemoveDir(*romsetPath + ui->SelectRomSet->currentText()))
            QMessageBox::critical(this,tr("Löschen?"),tr("RomSet konnte nicht gelöscht werden"));

        FillRomSetCombo();
    }
}

bool SetupWindow::RemoveDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists()) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = RemoveDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = QDir().rmdir(dirName);
    }
    return result;
}

void SetupWindow::on_NewRomSet_clicked()
{
    NewRomSetWindow *new_romset_window = new NewRomSetWindow(this,romsetPath);

    if(new_romset_window->exec())
    {
        // Neues Romset erstellen
        if(new_romset_window->GetRomSetName() != "")
        {
            QString NewRomSetDir = *romsetPath + new_romset_window->GetRomSetName();
            QDir romset_dir(NewRomSetDir);
            if(!romset_dir.exists())
            {
                if(romset_dir.mkdir(NewRomSetDir))
                {
                    // Verzeichnis wurde erstellt
                    QString Source, Target;

                    // Kernal kopieren
                    Source = new_romset_window->GetKernalRomFilename();
                    if(Source == "")
                        Source = *dataPath+"/roms/kernal.rom";

                    Target = NewRomSetDir + "/kernal.rom";
                    QFile::copy(Source, Target);

                    // Basic kopieren
                    Source = new_romset_window->GetBasicRomFilename();
                    if(Source == "")
                        Source = *dataPath+"/roms/basic.rom";
                    Target = NewRomSetDir + "/basic.rom";
                    QFile::copy(Source, Target);

                    // Char kopieren
                    Source = new_romset_window->GetCharRomFilename();
                    if(Source == "")
                        Source = *dataPath+"/roms/char.rom";
                    Target = NewRomSetDir + "/char.rom";
                    QFile::copy(Source, Target);

                    // 1541 kopieren
                    Source = new_romset_window->GetDos1541RomFilename();
                    if(Source == "")
                        Source = *dataPath+"/roms/1541.rom";
                    Target = NewRomSetDir + "/1541.rom";
                    QFile::copy(Source, Target);

                    FillRomSetCombo();
                    int idx = ui->SelectRomSet->findText(new_romset_window->GetRomSetName());
                    ui->SelectRomSet->setCurrentIndex(idx);

                }
                else
                    QMessageBox::critical(this,"Fehler...","Es konnte das ROM-Set Verzeichnis nicht erstellt werden.");
            }
        }
    }
    else
    {
        // Dialog abgebrochen

    }

    delete new_romset_window;
}

void SetupWindow::on_VicSpriteViewEnable_toggled(bool checked)
{
    c64->SetVicConfig(VIC_SPRITES_ON, checked);
}

void SetupWindow::on_VicBorderViewEnable_toggled(bool checked)
{
    c64->SetVicConfig(VIC_BORDER_ON, checked);
}

void SetupWindow::on_VicSprSprCollisionEnable_toggled(bool checked)
{
    c64->SetVicConfig(VIC_SPR_SPR_COLL_ON, checked);
}

void SetupWindow::on_VicSprBgrCollisionEnable_toggled(bool checked)
{
    c64->SetVicConfig(VIC_SPR_BCK_COLL_ON, checked);
}

void SetupWindow::on_VicGreyDotsEnable_toggled(bool checked)
{
    c64->SetVicConfig(VIC_GREY_DOTS_ON, checked);
}

void SetupWindow::on_default_pal_c64_clicked()
{
    ui->cycles_per_second->setValue(985248);
}

void SetupWindow::on_default_50hz_clicked()
{
    ui->cycles_per_second->setValue(982800);
}

void SetupWindow::on_cycles_per_second_valueChanged(int arg1)
{
    c64->SetC64Frequency(arg1);
}

void SetupWindow::on_Vsync_clicked(bool checked)
{
    c64->SetVSync(checked);
}

void SetupWindow::on_DisplayList_currentIndexChanged(int index)
{
	int num_video_modes = c64->GetNumDisplayModes(index);

	int w, h, refresh_rate;
	uint32_t format;

	is_filled_display_mode_list = true;
	ui->VideoModes->clear();
	for(int i = 0; i < num_video_modes; i++)
	{
		c64->GetDisplayMode(index, i, w, h, refresh_rate, format);
		ui->VideoModes->addItem(QString::number(w) + " x " + QString::number(h) + " " + QString::number(SDL_BITSPERPIXEL(format)) + "-Bits" + " " + QString::number(refresh_rate) + "Hz");
	}
	is_filled_display_mode_list = false;

	ui->VideoModes->setCurrentIndex(video_display_mode_index[index]);
}

void SetupWindow::on_VideoModes_currentIndexChanged(int index)
{
	if(c64 != nullptr)
	{
		c64->SetFullscreenDisplayMode(ui->DisplayList->currentIndex(),index);

		if(!is_filled_display_mode_list)
			video_display_mode_index[ui->DisplayList->currentIndex()] = index;
	}
}

void SetupWindow::on_SettingUserPalette_clicked()
{
	UserPaletteWindow *user_palette_window = new UserPaletteWindow(this);

	char key_name[32];
	QColor default_color = Qt::white;

	ini->beginGroup("C64UserPalette");
	for(int i=0; i<16; i++)
	{
		sprintf(key_name, "Color_%2.2d", i);

		default_color.setRed(default_palette[i*4+0]);
		default_color.setGreen(default_palette[i*4+1]);
		default_color.setBlue(default_palette[i*4+2]);

		QColor color = ini->value(key_name, default_color.rgba()).toUInt();
		user_palette_window->SetDefaultColor(i,default_color);
		user_palette_window->SetColor(i,color);
	}
	ini->endGroup();

	if(user_palette_window != nullptr)
	{
		QColor old_color[16];
		for(int i=0; i<16; i++)
			old_color[i] = user_palette_window->GetColor(i);

		connect(user_palette_window, SIGNAL(ChangeColor(int,QColor)), this, SLOT(OnChangeUserColor(int, QColor)));

		if(user_palette_window->exec())
		{
			// OK
			ini->beginGroup("C64UserPalette");
			for(int i=0; i<16; i++)
			{
				sprintf(key_name, "Color_%2.2d", i);

				QColor color = user_palette_window->GetColor(i);
				ini->setValue(key_name, color.rgba());
			}
			ini->endGroup();

			for(int i=0; i<16; i++)
			{
				QColor color = user_palette_window->GetColor(i);
				video_crt_output->SetUserPaletteColor(i, color.red(), color.green(), color.blue());
			}
		}
		else
		{
			// Cancel
			for(int i=0; i<16; i++)
				video_crt_output->SetUserPaletteColor(i,old_color[i].red(), old_color[i].green(), old_color[i].blue());
		}

		disconnect(user_palette_window, SIGNAL(ChangeColor(int,QColor)), this, SLOT(OnChangeUserColor(int, QColor)));

		delete user_palette_window;
	}
}

void SetupWindow::on_EnableUserPalette_clicked(bool checked)
{
	video_crt_output->EnableUserPalette(checked);
}

void SetupWindow::OnChangeUserColor(int color_number, QColor color)
{
	video_crt_output->SetUserPaletteColor(color_number, color.red(), color.green(), color.blue());
}
