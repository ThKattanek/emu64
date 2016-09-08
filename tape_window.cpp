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
// Letzte Änderung am 08.09.2016                //
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

    connect(ui->FileBrowser,SIGNAL(select_file(QString)),this,SLOT(OnSelectFile(QString)));

    ui->FileBrowser->SetFileFilter(QStringList()<<"*.tap"<<"*.wav");

    isOneShowed = false;

    this->c64 = c64;
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

void TapeWindow::showEvent(QShowEvent *event)
{
    isOneShowed = true;
}

void TapeWindow::OnSelectFile(QString filename)
{
    UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_STOP));
    if(!c64->LoadTapeImage(filename.toAscii().data()))
        QMessageBox::warning(this,trUtf8("Fehler!"),trUtf8("Fehler beim laden des TapeImages"));
}

void TapeWindow::on_Rec_clicked()
{
}

void TapeWindow::on_Play_clicked()
{
    UpdateStateTapeKeys(c64->SetTapeKeys(TAPE_KEY_PLAY));
}

void TapeWindow::on_Rew_clicked()
{
}

void TapeWindow::on_FFw_clicked()
{
}

void TapeWindow::on_Stop_clicked()
{
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
