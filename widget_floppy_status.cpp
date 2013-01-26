//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: widget_floppy_status.cpp              //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 12.10.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include <QFontDatabase>
#include <QTimer>

#include "widget_floppy_status.h"
#include "ui_widget_floppy_status.h"

WidgetFloppyStatus::WidgetFloppyStatus(QWidget *parent, int floppy_nr, Floppy1541 *_floppy) :
    QWidget(parent),
    ui(new Ui::WidgetFloppyStatus),
    PowerLED(false),
    MotorLED(false),
    RWLED(false)
{
    FloppyNr = floppy_nr;
    floppy = _floppy;
    ui->setupUi(this);

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/emu64.ttf");
    QFont font("Emu64 D64 Directory",16);
    font.setBold(false);
    ui->DisknameOut->setFont(font);

    iGreenLedOff = new QIcon(":/grafik/green_off_32.png");
    iGreenLedOn = new QIcon(":/grafik/green_on_32.png");

    iYellowLedOff = new QIcon(":/grafik/yellow_off_32.png");
    iYellowLedOn = new QIcon(":/grafik/yellow_on_32.png");

    iVol_mute = new QIcon(":/grafik/audio_volume_mute.png");
    iVol_low = new QIcon(":/grafik/audio_volume_low.png");
    iVol_medium = new QIcon(":/grafik/audio_volume_medium.png");
    iVol_high = new QIcon(":/grafik/audio_volume_high.png");

    SetFloppyVolume(2);

    ui->PowerLED->setIcon(*iGreenLedOff);
    ui->MotorLED->setIcon(*iGreenLedOff);
    ui->RWLED->setIcon(*iYellowLedOff);

    old_MotorLED = false;
    old_RWLED = false;
    old_Sektor = 0;
    old_Spur = 0;

    timer = new QTimer(this);
    timer->setInterval(20);

    connect(timer,SIGNAL(timeout()),this,SLOT(onTimer()));
}

WidgetFloppyStatus::~WidgetFloppyStatus()
{
    delete timer;
    delete ui;
}

void WidgetFloppyStatus::onTimer()
{
    FLOPPY_INFO info;
     char str[10];

    floppy->GetFloppyInfo(&info);

    if(old_MotorLED != info.Motor)
    {
        if(info.Motor) ui->MotorLED->setIcon(*iGreenLedOn);
        else ui->MotorLED->setIcon(*iGreenLedOff);
        old_MotorLED = info.Motor;
    }

    if(old_RWLED != info.Data)
    {
        if(info.Data) ui->RWLED->setIcon(*iYellowLedOn);
        else ui->RWLED->setIcon(*iYellowLedOff);
        old_RWLED = info.Data;
    }

    if(old_Spur != (info.Spur))
    {
        sprintf(str,"%2.2d",info.Spur+1);
        ui->TrackOut->setText(str);
        old_Spur = info.Spur;
    }

    if(old_Sektor != info.Sektor)
    {
        sprintf(str,"%2.2d",info.Sektor);
        ui->SekcorOut->setText(str);
        old_Sektor = info.Sektor;
    }
}

void WidgetFloppyStatus::SetGeraeteID(unsigned char id)
{
    GeraeteID = id;
    char str[10];
    sprintf(str,"%2.2d",id);
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
        ui->MotorLED->setIcon(*iGreenLedOff);
        ui->RWLED->setIcon(*iYellowLedOff);
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
