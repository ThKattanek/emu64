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
// Letzte Änderung am 25.09.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "tape_window.h"
#include "ui_tape_window.h"

TapeWindow::TapeWindow(QWidget *parent, QSettings *_ini, C64Class *c64) :
    QDialog(parent),
    ui(new Ui::TapeWindow)
{
    ini = _ini;
    ui->setupUi(this);

    // ICON's für LED's laden
    GreenLEDOff = new QIcon(":/grafik/green_led_off.png");
    GreenLEDOn = new QIcon(":/grafik/green_led_on.png");
    RedLEDOff = new QIcon(":/grafik/red_led_off.png");
    RedLEDOn = new QIcon(":/grafik/red_led_on.png");

    ui->MotorLED->setIcon(*GreenLEDOff);
    ui->RecordLED->setIcon(*RedLEDOff);

    // Filebrowser initialisieren
    connect(ui->FileBrowser,SIGNAL(select_file(QString)),this,SLOT(OnSelectFile(QString)));
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
            ini->setValue("AktDir",ui->FileBrowser->GetAktDir());
            ini->setValue("AktFile",ui->FileBrowser->GetAktFile());
        }
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
    isOneShowed = false;
    refresh_timer->stop();
}

void TapeWindow::OnSelectFile(QString filename)
{
    UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_STOP));
    if(!c64->LoadTapeImage(filename.toAscii().data()))
        QMessageBox::warning(this,trUtf8("Fehler!"),trUtf8("Fehler beim laden des TapeImages"));

    unsigned int tape_time = roundf(c64->GetTapeLenTime());

    QString TimeString = "";
    TimeString += QVariant(tape_time / 3600).toString() + "h ";
    tape_time %= 3600;
    TimeString += QVariant(tape_time / 60).toString() + "m ";
    tape_time %= 60;
    TimeString += QVariant(tape_time).toString() + "s";

    ui->TapeInfoOut->setText(trUtf8("Tape Länge: ") + TimeString + " [MaxCounter: " + QVariant(c64->GetTapeLenCount()).toString() + "]");
}

void TapeWindow::OnRefreshGUI()
{
    // Counter setzen
    ui->Counter->SetCounter(c64->GetTapeCounter());

    // LED's updaten
    if(c64->GetTapeMotorStatus()) ui->MotorLED->setIcon(*GreenLEDOn);
    else ui->MotorLED->setIcon(*GreenLEDOff);
}

void TapeWindow::on_Rec_clicked()
{
    UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_REC));
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
}

void TapeWindow::UpdateStateTapeKeys(unsigned char key_pressed)
{
    if(key_pressed & TAPE_KEY_REC) ui->Rec->setChecked(true);
    else ui->Rec->setChecked(false);
    if(key_pressed & TAPE_KEY_PLAY) ui->Play->setChecked(true);
    else ui->Play->setChecked(false);
    if(key_pressed & TAPE_KEY_REW) ui->Rew->setChecked(true);
    else ui->Rew->setChecked(false);
    if(key_pressed & TAPE_KEY_FFW) ui->FFw->setChecked(true);
    else ui->FFw->setChecked(false);
    if(key_pressed & TAPE_KEY_STOP) ui->Stop->setChecked(true);
    else ui->Stop->setChecked(false);
}
