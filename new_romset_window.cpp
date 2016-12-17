//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: new_romset_window.cpp                 //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 17.12.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "new_romset_window.h"
#include "ui_new_romset_window.h"

NewRomSetWindow::NewRomSetWindow(QWidget *parent, QString *romsetPath) :
    QDialog(parent),
    ui(new Ui::NewRomSetWindow)
{
    this->romsetPath = romsetPath;

    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

NewRomSetWindow::~NewRomSetWindow()
{
    delete ui;
}

QString NewRomSetWindow::GetRomSetName()
{
    return ui->RomSetName->text();
}

QString NewRomSetWindow::GetKernalRomFilename()
{
    return ui->kernal_rom->text();
}

QString NewRomSetWindow::GetBasicRomFilename()
{
    return ui->basic_rom->text();
}

QString NewRomSetWindow::GetCharRomFilename()
{
    return ui->char_rom->text();
}

QString NewRomSetWindow::GetDos1541RomFilename()
{
    return ui->dos1514_rom->text();
}

void NewRomSetWindow::on_sel_kernal_rom_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,trUtf8("Kernal ROM öffnen "),"",trUtf8("ROM Dateien ") + "(*.rom);;" + trUtf8("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
        ui->kernal_rom->setText(filename);

        QFile file(filename);
        if(file.size() != 8192)
            ui->KernalSizeError->setText(trUtf8("Falsche Dateigröße !"));
        else ui->KernalSizeError->setText(trUtf8(""));
    }
}

void NewRomSetWindow::on_sel_basic_rom_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,trUtf8("Basic ROM öffnen "),"",trUtf8("ROM Dateien ") + "(*.rom);;" + trUtf8("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
        ui->basic_rom->setText(filename);

        QFile file(filename);
        if(file.size() != 8192)
            ui->BasicSizeError->setText(trUtf8("Falsche Dateigröße !"));
        else ui->BasicSizeError->setText(trUtf8(""));
    }
}

void NewRomSetWindow::on_sel_char_rom_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,trUtf8("Char ROM öffnen "),"",trUtf8("ROM Dateien ") + "(*.rom);;" + trUtf8("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
        ui->char_rom->setText(filename);

        QFile file(filename);
        if(file.size() != 4096)
            ui->CharSizeError->setText(trUtf8("Falsche Dateigröße !"));
        else ui->CharSizeError->setText(trUtf8(""));
    }
}

void NewRomSetWindow::on_sel_dos1541_rom_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,trUtf8("Dos 1541 ROM öffnen "),"",trUtf8("ROM Dateien ") + "(*.rom);;" + trUtf8("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
        ui->dos1514_rom->setText(filename);

        QFile file(filename);
        if(file.size() != 16384)
            ui->DosSizeError->setText(trUtf8("Falsche Dateigröße !"));
        else ui->DosSizeError->setText(trUtf8(""));
    }
}

void NewRomSetWindow::on_RomSetName_textChanged(const QString &arg1)
{
    if(arg1 == "")
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return;
    }
    else
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);


    QDir dir(*romsetPath + arg1);

    if(dir.exists())
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        ui->ExistInfo->setText(trUtf8("ROM-Set existiert schon!"));
    }
    else
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        ui->ExistInfo->setText(trUtf8(""));
    }
}
