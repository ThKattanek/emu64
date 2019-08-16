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
// Letzte Änderung am 16.08.2019                //
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
    QString filename = QFileDialog::getOpenFileName(this,tr("Kernal ROM öffnen "),"",tr("ROM Dateien ") + "(*.rom);;" + tr("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
        ui->kernal_rom->setText(filename);

        QFile file(filename);
        if(file.size() != 8192)
            ui->KernalSizeError->setText(tr("Falsche Dateigröße !"));
        else ui->KernalSizeError->setText(tr(""));
    }
}

void NewRomSetWindow::on_sel_basic_rom_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Basic ROM öffnen "),"",tr("ROM Dateien ") + "(*.rom);;" + tr("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
        ui->basic_rom->setText(filename);

        QFile file(filename);
        if(file.size() != 8192)
            ui->BasicSizeError->setText(tr("Falsche Dateigröße !"));
        else ui->BasicSizeError->setText(tr(""));
    }
}

void NewRomSetWindow::on_sel_char_rom_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Char ROM öffnen "),"",tr("ROM Dateien ") + "(*.rom);;" + tr("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
        ui->char_rom->setText(filename);

        QFile file(filename);
        if(file.size() != 4096)
            ui->CharSizeError->setText(tr("Falsche Dateigröße !"));
        else ui->CharSizeError->setText(tr(""));
    }
}

void NewRomSetWindow::on_sel_dos1541_rom_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Dos 1541 ROM öffnen "),"",tr("ROM Dateien ") + "(*.rom);;" + tr("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
        ui->dos1514_rom->setText(filename);

        QFile file(filename);
        if(file.size() != 16384)
            ui->DosSizeError->setText(tr("Falsche Dateigröße !"));
        else ui->DosSizeError->setText(tr(""));
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
        ui->ExistInfo->setText(tr("ROM-Set existiert schon!"));
    }
    else
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        ui->ExistInfo->setText(tr(""));
    }
}
