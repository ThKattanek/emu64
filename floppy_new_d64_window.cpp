//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy_new_d64_window.cpp             //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 09.08.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "floppy_new_d64_window.h"
#include "ui_floppy_new_d64_window.h"

FloppyNewD64Window::FloppyNewD64Window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FloppyNewD64Window)
{
    ui->setupUi(this);

    QFontDatabase::addApplicationFont(":/fonts/emu64.ttf");
    c64_font1 = new QFont("Emu64 D64 Directory",18);
    c64_font1->setStyleStrategy(QFont::PreferAntialias);
    c64_font1->setBold(false);
    c64_font1->setKerning(true);

    ui->Diskname->setFont(*c64_font1);
    ui->DIskID->setFont(*c64_font1);

    ui->Filename->setText(trUtf8("Neue_Disk"));
    ui->Diskname->setText(trUtf8("EMU64 DISK"));
    ui->DIskID->setText("01");
}

FloppyNewD64Window::~FloppyNewD64Window()
{
    delete ui;
}

QString FloppyNewD64Window::GetFilename()
{
    return ui->Filename->text();
}

QString FloppyNewD64Window::GetDiskname()
{
    return ui->Diskname->text();
}

QString FloppyNewD64Window::GetDiskID()
{
    return ui->DIskID->text();
}

void FloppyNewD64Window::on_Diskname_textChanged(const QString &arg1)
{
    int cursor_pos = ui->Diskname->cursorPosition();
    ui->Diskname->setText(arg1.toUpper());
    ui->Diskname->setCursorPosition(cursor_pos);
}

void FloppyNewD64Window::on_DIskID_textChanged(const QString &arg1)
{
    int cursor_pos = ui->Diskname->cursorPosition();
    ui->DIskID->setText(arg1.toUpper());
    ui->DIskID->setCursorPosition(cursor_pos);
}
