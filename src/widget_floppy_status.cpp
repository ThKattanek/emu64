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

    int id = QFontDatabase::addApplicationFont(":/fonts/C64_Pro-STYLE.ttf");

    QString family = "C64 Pro"; // Fallback
    if (id != -1) {
        const QStringList families = QFontDatabase::applicationFontFamilies(id);
        if (!families.isEmpty())
            family = families.first(); // echte Familie aus der TTF
    }

    QFont font1(family, 8);

    font1.setPixelSize(8);
    font1.setStyleStrategy(QFont::PreferAntialias);
    font1.setBold(false);
    font1.setKerning(true);

    ui->DisknameOut->setFont(font1);

    // Festbreiten-Schrift und feste Breite für Spur und Sektor, damit der Diskname bei wechselnden Werten nicht springt
    {
        QFont mono_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        mono_font.setPointSize(12);
        ui->TrackOut->setFont(mono_font);
        ui->Label1->setFont(mono_font);
        ui->SekcorOut->setFont(mono_font);

        const QFontMetrics fm_track(ui->TrackOut->font());
        ui->TrackOut->setFixedWidth(fm_track.boundingRect("88.8").width() + 2);

        const QFontMetrics fm_sector(ui->SekcorOut->font());
        ui->Label1->setFixedWidth(QFontMetrics(ui->Label1->font()).boundingRect("-").width() + 2);
        ui->SekcorOut->setFixedWidth(qMax(fm_sector.boundingRect("88").width(), fm_sector.boundingRect("--").width()) + 2);
    }

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
    ShowFloppyOff();

    old_Sektor = 255;
    old_Spur = 255;
    old_Bereich = 255;
    pending_Spur = 255;
    pending_Ticks = 0;

    timer = new QTimer(this);
	timer->setInterval(10);

    connect(timer,SIGNAL(timeout()),this,SLOT(onTimer()));
}

WidgetFloppyStatus::~WidgetFloppyStatus()
{
    delete timer;
    delete ui;

    if(iGreenLedOff != nullptr) delete iGreenLedOff;
    if(iGreenLedOn != nullptr) delete iGreenLedOn;
    if(iVol_mute != nullptr) delete iVol_mute;
    if(iVol_low != nullptr) delete iVol_low;
    if(iVol_medium != nullptr) delete iVol_medium;
    if(iVol_high != nullptr) delete iVol_high;
    if(iError != nullptr) delete iError;
}

// Anzeige bei ausgeschalteter Floppy: "01.0 - --"
void WidgetFloppyStatus::ShowFloppyOff()
{
    ui->TrackOut->setText("01.0");
    ui->Label1->setText("-");
    ui->SekcorOut->setText("--");
    ui->SekcorOut->setToolTip("");
    ui->TrackOut->setStatusTip("");
    ui->SekcorOut->setStatusTip("");

    old_Spur = 255;
    old_Sektor = 255;
    old_Bereich = 255;
    pending_Spur = 255;
    pending_Ticks = 0;
}

void WidgetFloppyStatus::RetranslateUi()
{
    ui->retranslateUi(this);
    if(!PowerLED)
        ShowFloppyOff();
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

    // Spur immer als "NN.0" oder "NN.5" (feste Breite). Das DOS fährt Halbspuren nur im Durchgang an,
    // eine ungerade Halbspur wird deshalb erst angezeigt, wenn der Kopf dort ca. 50ms stehen bleibt.
    uint8_t halb_spur = (info.Spur << 1) | info.HalbSpur;
    if(info.HalbSpur)
    {
        if(pending_Spur != halb_spur)
        {
            pending_Spur = halb_spur;
            pending_Ticks = 0;
        }
        if(pending_Ticks < 5 && old_Spur != 255)
        {
            pending_Ticks++;
            halb_spur = old_Spur;   // Noch die zuletzt angezeigte Spur beibehalten
        }
    }
    else
    {
        pending_Spur = 255;
        pending_Ticks = 0;
    }

    if(old_Spur != halb_spur)
    {
        unsigned int spur = halb_spur >> 1;
        if(spur > 99)
            spur = 99;
        snprintf(str,MAX_STRLEN,"%02u.%u",spur,static_cast<unsigned int>(halb_spur & 1));
        ui->TrackOut->setText(str);
        old_Spur = halb_spur;
    }

    // Sektor unter dem Kopf (nur bei laufendem Motor)
    const uint8_t sektor = info.Motor ? info.Sektor : FLOPPY_SECTOR_NONE;
    if(old_Sektor != sektor)
    {
        if(sektor == FLOPPY_SECTOR_NONE)
            ui->SekcorOut->setText("--");
        else
        {
            snprintf(str,MAX_STRLEN,"%02u",static_cast<unsigned int>(sektor));
            ui->SekcorOut->setText(str);
        }
        old_Sektor = sektor;
    }

    // Bereich unter dem Kopf als Tooltip
    const uint8_t bereich = info.Motor ? info.Bereich : static_cast<uint8_t>(FLOPPY_AREA_NONE);
    if(old_Bereich != bereich)
    {
        switch(bereich)
        {
        case FLOPPY_AREA_HEADER:
            ui->SekcorOut->setToolTip(tr("Kopf über einem Sektor-Header."));
            break;
        case FLOPPY_AREA_HEADER_GAP:
            ui->SekcorOut->setToolTip(tr("Kopf zwischen Sektor-Header und Datenblock."));
            break;
        case FLOPPY_AREA_DATA:
            ui->SekcorOut->setToolTip(tr("Kopf über einem Datenblock."));
            break;
        case FLOPPY_AREA_GAP:
            ui->SekcorOut->setToolTip(tr("Kopf in der Lücke nach einem Sektor."));
            break;
        default:
            ui->SekcorOut->setToolTip(tr("Kein Sektor unter dem Kopf (Motor aus, keine Diskette oder unformatierte Spur)."));
            break;
        }
        old_Bereich = bereich;
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
        ShowFloppyOff();
    }
    else
    {
        PowerLED = true;
        ui->PowerLED->setIcon(*iGreenLedOn);
        ui->TrackOut->setStatusTip(tr("Aktuelle Spur."));
        ui->SekcorOut->setStatusTip(tr("Aktueller Sektor."));
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

