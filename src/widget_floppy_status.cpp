//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_floppy_status.cpp              //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// https://github.com/ThKattanek/emu64          //
//                                              //
//////////////////////////////////////////////////

#include <QFontDatabase>
#include <QMessageBox>
#include <QTimer>

#include "widget_floppy_status.h"
#include "ui_widget_floppy_status.h"
#include "dos_error_messages.h"

WidgetFloppyStatus::WidgetFloppyStatus(QWidget *parent, int floppy_nr, Floppy1541 *_floppy) :
    QWidget(parent),
    ui(new Ui::WidgetFloppyStatus),
    PowerLED(false),
    MotorLED(false),
    RWLED(false)
{
    FloppyNr = floppy_nr;
    floppy = _floppy;

    old_Sektor = 255;
    old_Spur = 255;

    ui->setupUi(this);

    QFontDatabase fontDB;

    fontDB.addApplicationFont(":/fonts/C64_Pro-STYLE.ttf");
    QFont font("C64 Pro");
    font.setPixelSize(8);
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setBold(false);
    font.setKerning(true);

    ui->DisknameOut->setFont(font);

    iGreenLedOff = new QIcon(":/grafik/green_off_32.png");
    iGreenLedOn = new QIcon(":/grafik/green_on_32.png");

    iVol_mute = new QIcon(":/grafik/audio_volume_mute.png");
    iVol_low = new QIcon(":/grafik/audio_volume_low.png");
    iVol_medium = new QIcon(":/grafik/audio_volume_medium.png");
    iVol_high = new QIcon(":/grafik/audio_volume_high.png");

	// Error
	iError = new QIcon(":/grafik/warning.png");
	ui->Error->setIcon(*iError);
	ui->Error->setToolTip(tr("Fehlerkanal anzeigen."));
	ui->Error->setStatusTip("Fehlerkanal anzeigen.");
	ui->Error->hide();

	/// yellow
	ui->rw_led->SetColorOff(QColor(70,60,20));
	ui->rw_led->SetColorOn(QColor(240,230,120));
	// green
	ui->motor_led->SetColorOff(QColor(20,60,20));
	ui->motor_led->SetColorOn(QColor(50,240,20));

    SetFloppyVolume(2);

    ui->PowerLED->setIcon(*iGreenLedOff);

    old_Sektor = 0;
    old_Spur = 0;

    timer = new QTimer(this);
	timer->setInterval(10);

    connect(timer,SIGNAL(timeout()),this,SLOT(onTimer()));
}

WidgetFloppyStatus::~WidgetFloppyStatus()
{
    delete timer;
    delete ui;
}

void WidgetFloppyStatus::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void WidgetFloppyStatus::onTimer()
{
    FLOPPY_INFO info;
    #define MAX_STRLEN 16
    char str[MAX_STRLEN+1] = "";

    floppy->GetFloppyInfo(&info);

	if(info.Motor)
		ui->motor_led->SetBrightness(1.0f);
	else
		ui->motor_led->SetBrightness(0.0f);

	ui->rw_led->SetBrightness(info.Data_RMS);

    if(old_Spur != (info.Spur))
    {
        int spur = info.Spur+1;
        if(spur > 99)
            spur = 99;
        snprintf(str,MAX_STRLEN,"%02d",static_cast<unsigned int>(spur));
        ui->TrackOut->setText(str);
        old_Spur = info.Spur;
    }

    if(old_Sektor != info.Sektor)
    {
        snprintf(str,MAX_STRLEN,"%02d",static_cast<unsigned int>(info.Sektor));
        ui->SekcorOut->setText(str);
        old_Sektor = info.Sektor;
    }

	// Icon Anzeigen wenn Floppy Ram $026D != 0
	if(info.ErrorFlag != 0)
		ui->Error->show();
	else
		ui->Error->hide();
}

void WidgetFloppyStatus::SetGeraeteID(unsigned char id)
{
    GeraeteID = id;
    char str[10];
    sprintf(str,"%02d",id);
    ui->GeraeteIDOut->setText(str);
}

void WidgetFloppyStatus::on_PowerLED_clicked()
{
    PowerLED = !PowerLED;
    SetEnableFloppy(PowerLED);
}

void WidgetFloppyStatus::on_InstertImage_clicked()
{
    emit LoadImage(GeraeteID-8);
}

void WidgetFloppyStatus::on_EjectImage_clicked()
{
    floppy->UnLoadDiskImage();

    AktFileName = "";
    ui->DisknameOut->setText(AktFileName);
    ui->DisknameOut->setToolTip(AktFileName);

    emit RemoveImage(GeraeteID-8);
}

QString WidgetFloppyStatus::GetAktFilename(void)
{
    return AktFileName;
}

bool WidgetFloppyStatus::GetEnableFloppy(void)
{
    return floppy->GetEnableFloppy();
}

void WidgetFloppyStatus::SetAktFilename(QString filename, QString d64name)
{
    AktFileName = filename;
    ui->DisknameOut->setText(d64name);
    ui->DisknameOut->setToolTip(AktFileName);
}

void WidgetFloppyStatus::SetEnableFloppy(bool status)
{
    if(!status)
    {
        PowerLED = false;
        ui->PowerLED->setIcon(*iGreenLedOff);
		ui->motor_led->SetBrightness(0.0f);
		ui->rw_led->SetBrightness(0.0f);
		ui->Error->hide();

        floppy->SetEnableFloppy(false);
        timer->stop();
    }
    else
    {
        PowerLED = true;
        ui->PowerLED->setIcon(*iGreenLedOn);
        floppy->SetEnableFloppy(true);
        timer->start();
    }
    emit ChangeEnableFloppy();
}

void WidgetFloppyStatus::on_Volume_clicked()
{
    if(FloppySoundVolumeMode == 3) FloppySoundVolumeMode = 0;
    else FloppySoundVolumeMode++;
	SetFloppyVolume(FloppySoundVolumeMode);
}

void WidgetFloppyStatus::SetFloppyVolume(int mode)
{
    FloppySoundVolumeMode = mode;

    switch(mode)
    {
    case 0:
        ui->Volume->setIcon(*iVol_mute);
        floppy->SetFloppySoundVolume(0.0);
        floppy->SetEnableFloppySound(false);
        ui->Volume->setToolTip(tr("Floppygeräusche sind ausgeschaltet."));
        break;
    case 1:
        ui->Volume->setIcon(*iVol_low);
        floppy->SetFloppySoundVolume(0.3);
        floppy->SetEnableFloppySound(true);
        ui->Volume->setToolTip(tr("Floppygeräusche sind auf 30% gestellt."));
        break;
    case 2:
        ui->Volume->setIcon(*iVol_medium);
        floppy->SetFloppySoundVolume(0.7);
        floppy->SetEnableFloppySound(true);
        ui->Volume->setToolTip(tr("Floppygeräusche sind auf 70% gestellt."));
        break;
    case 3:
        ui->Volume->setIcon(*iVol_high);
        floppy->SetFloppySoundVolume(1.0);
        floppy->SetEnableFloppySound(true);
        ui->Volume->setToolTip(tr("Floppygeräusche sind auf 100% gestellt."));
        break;
    default:
        break;
    }
}

int WidgetFloppyStatus::GetFloppyVolume(void)
{
    return FloppySoundVolumeMode;
}

void WidgetFloppyStatus::on_Error_clicked()
{
	FLOPPY_INFO fi;
	floppy->GetFloppyInfo(&fi);

	uint8_t error_number = (fi.ErrorMsg[0] - 0x30) * 10 + (fi.ErrorMsg[1] - 0x30);

	int i=0;
	bool found = false;
	while(i < DOS_ERROR_COUNT && !found)
	{
		if(dos_error_numbers[i] == error_number)
			found = true;
		else
			i++;
	}

	if(found)
		QMessageBox::warning(this,"Floppy #" + QVariant(FloppyNr + 8).toString() + tr(" Fehlerkanal"), dos_error_msg[i]);
}

