//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tape_window.cpp                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 13.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "tape_window.h"
#include "ui_tape_window.h"

TapeWindow::TapeWindow(QWidget *parent, QSettings *_ini, C64Class *c64, QString tmp_path) :
    QDialog(parent),
    ui(new Ui::TapeWindow)
{
    ini = _ini;
    ui->setupUi(this);

    this->tmp_path = tmp_path;
    ui->FileBrowser->SetTempDir(tmp_path);

    // ICON's für LED's laden
    GreenLEDOff = new QIcon(":/grafik/green_led_off.png");
    GreenLEDOn = new QIcon(":/grafik/green_led_on.png");
    RedLEDOff = new QIcon(":/grafik/red_led_off.png");
    RedLEDOn = new QIcon(":/grafik/red_led_on.png");

    ui->MotorLED->setIcon(*GreenLEDOff);
    ui->RecordLED->setIcon(*RedLEDOff);

    VolMute = new QIcon(":/grafik/audio_volume_mute.png");
    VolLow = new QIcon(":/grafik/audio_volume_low.png");
    VolMedium = new QIcon(":/grafik/audio_volume_medium.png");
    VolHigh = new QIcon(":/grafik/audio_volume_high.png");

    c64->SetTapeSoundVolume(0.0);
    TapeSoundVolumeMode = 0;
    ui->Volume->setIcon(*VolMute);

    // Filebrowser initialisieren
    connect(ui->FileBrowser,SIGNAL(SelectFile(QString)),this,SLOT(OnSelectFile(QString)));
    //ui->FileBrowser->SetFileFilter(QStringList()<<"*.tap"<<"*.wav");
    ui->FileBrowser->SetFileFilter(QStringList()<<"*.tap");

    // Wird momentan nicht angezeigt
    isOneShowed = false;

    this->c64 = c64;

    refresh_timer = new QTimer(this);
    refresh_timer->setInterval(40);    // 40ms also 25x in der Sekunde
    refresh_timer->stop();
    connect(refresh_timer,SIGNAL(timeout()),this,SLOT(OnRefreshGUI()));
}

TapeWindow::~TapeWindow()
{
    ////////// Save to INI ///////////
    if(ini != 0)
    {
        ini->beginGroup("TapeWindow");
        if(isOneShowed)
        {
            ini->setValue("Geometry",saveGeometry());
        }
        ini->setValue("AktDir",ui->FileBrowser->GetAktDir());
        ini->setValue("AktFile",ui->FileBrowser->GetAktFile());
        ini->endGroup();
    }
    ////////////////////////////////////

    delete refresh_timer;

    delete ui;
}

void TapeWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    ui->FileBrowser->RetranslateUi();
    this->update();
}

void TapeWindow::LoadIni()
{
    ////////// Load from INI ///////////
    if(ini != 0)
    {
        ini->beginGroup("TapeWindow");
        if(ini->contains("Geometry")) restoreGeometry(ini->value("Geometry").toByteArray());
        ui->FileBrowser->SetAktDir(ini->value("AktDir","").toString());
        ui->FileBrowser->SetAktFile(ini->value("AktDir","").toString(),ini->value("AktFile","").toString());
        ini->endGroup();
    }
    ////////////////////////////////////
}

void TapeWindow::showEvent(QShowEvent*)
{
    isOneShowed = true;
    refresh_timer->start();
}

void TapeWindow::hideEvent(QHideEvent*)
{
    refresh_timer->stop();
}

void TapeWindow::OnSelectFile(QString filename)
{
    UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_STOP));
    if(!c64->LoadTapeImage(filename.toLocal8Bit()))
        QMessageBox::warning(this,tr("Fehler!"),tr("Fehler beim laden des TapeImages"));

    unsigned int tape_time = roundf(c64->GetTapeLenTime());

    QString TimeString = "";
    TimeString += QVariant(tape_time / 3600).toString() + "h ";
    tape_time %= 3600;
    TimeString += QVariant(tape_time / 60).toString() + "m ";
    tape_time %= 60;
    TimeString += QVariant(tape_time).toString() + "s";

    ui->TapeInfoOut->setText(tr("Tape Länge: ") + TimeString + " [MaxCounter: " + QVariant(c64->GetTapeLenCount()).toString() + "]");
}

void TapeWindow::OnRefreshGUI()
{
    // Counter setzen
    ui->Counter->SetCounter(c64->GetTapeCounter());

    // LED's updaten
    if(c64->GetTapeMotorStatus()) ui->MotorLED->setIcon(*GreenLEDOn);
    else ui->MotorLED->setIcon(*GreenLEDOff);

    if(c64->GetTapeRecordLedStatus()) ui->RecordLED->setIcon(*RedLEDOn);
    else ui->RecordLED->setIcon(*RedLEDOff);
}

void TapeWindow::on_Rec_clicked()
{
    TapeNewWindow tnw(this);
    tnw.setWindowTitle(tr("Name des neues TAP Files."));

    if(tnw.exec())
    {
        QString filename = tnw.GetFilename();
        if(!(filename == ""))
        {
            AddFileExtension(filename, ".tap");

            QString fullpath = ui->FileBrowser->GetAktDir() + "/" + filename;

            if(QFile::exists(fullpath))
            {
                if(QMessageBox::No == QMessageBox::question(this,tr("Achtung!"),tr("Eine Datei mit diesen Namen existiert schon!\nSoll diese überschrieben werden?"),QMessageBox::Yes | QMessageBox::No))
                {
                    UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_STOP));
                    return;
                }
            }

            // Theoretisch bereit zur Aufnahme
            if(!c64->RecordTapeImage(fullpath.toLocal8Bit()))
            {
                QMessageBox::information(this,tr("Achtung"),tr("Es konnte keine Aufnahme gestartet werden.\nBitte überprüfen Sie ob sie ausreichend Rechte besitzen."));
            }
            else
            {
                UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_REC));
                ui->FileBrowser->RefreshAktDir();
                ui->FileBrowser->setEnabled(false);
            }
        }
        else
        {
            QMessageBox::information(this,tr("Achtung"),tr("Es muss ein Tape Name angegeben werden.\nEs wird keine Aufnahme gestartet!"));
            UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_STOP));
        }
    }
    else UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_STOP));
}

void TapeWindow::on_Play_clicked()
{
    UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_PLAY));
}

void TapeWindow::on_Rew_clicked()
{
    UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_REW));
}

void TapeWindow::on_FFw_clicked()
{
    UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_FFW));
}

void TapeWindow::on_Stop_clicked()
{
    UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_STOP));
    ui->FileBrowser->setEnabled(true);
    ui->FileBrowser->RefreshAktDir();
}

void TapeWindow::UpdateStateTapeKeys(unsigned char key_pressed)
{
    if(key_pressed & TAPE_KEY_PLAY) ui->Play->setChecked(true);
    else ui->Play->setChecked(false);
    if(key_pressed & TAPE_KEY_REW) ui->Rew->setChecked(true);
    else ui->Rew->setChecked(false);
    if(key_pressed & TAPE_KEY_FFW) ui->FFw->setChecked(true);
    else ui->FFw->setChecked(false);
    if(key_pressed & TAPE_KEY_STOP) ui->Stop->setChecked(true);
    else ui->Stop->setChecked(false);
    if(key_pressed & TAPE_KEY_REC)
    {
        ui->Rec->setChecked(true);
        ui->Play->setChecked(true);
    }
    else
    {
        ui->Rec->setChecked(false);
    }
}

void TapeWindow::on_Volume_clicked()
{
    if(TapeSoundVolumeMode == 3) TapeSoundVolumeMode = 0;
    else TapeSoundVolumeMode++;
    SetTapeVolume(TapeSoundVolumeMode);
}

void TapeWindow::SetTapeVolume(int mode)
{
    TapeSoundVolumeMode = mode;

    switch(mode)
    {
    case 0:
        ui->Volume->setIcon(*VolMute);
        c64->SetTapeSoundVolume(0.0);
        ui->Volume->setToolTip(tr("Datasette Lautsprecher ist ausgeschaltet."));
        break;
    case 1:
        ui->Volume->setIcon(*VolLow);
        c64->SetTapeSoundVolume(0.05);
        ui->Volume->setToolTip(tr("Datasette Lautsprecher ist auf leise gestellt."));
        break;
    case 2:
        ui->Volume->setIcon(*VolMedium);
        c64->SetTapeSoundVolume(0.1);
        ui->Volume->setToolTip(tr("Datasette Lautsprecher ist auf mittel gestellt."));
        break;
    case 3:
        ui->Volume->setIcon(*VolHigh);
        c64->SetTapeSoundVolume(0.3);
        ui->Volume->setToolTip(tr("Datasette Lautsprecher ist auf laut gestellt."));
        break;
    default:
        break;
    }
}

void TapeWindow::AddFileExtension(QString& filename, const QString extension)
{
    if(filename.length() > 4)
    {
        if(!(filename.right(4).toLower() == extension.toLower()))
            filename += extension;
    }
    else filename += extension;
}
