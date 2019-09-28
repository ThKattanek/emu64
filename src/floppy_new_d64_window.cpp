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
// Letzte Änderung am 28.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QScreen>
#include <QStyle>

#include "floppy_new_d64_window.h"
#include "ui_floppy_new_d64_window.h"

FloppyNewD64Window::FloppyNewD64Window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FloppyNewD64Window)
{
    ui->setupUi(this);

    // Center Window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::screens()[0]->availableGeometry()));

    QFontDatabase::addApplicationFont(":/fonts/C64_Pro-STYLE.ttf");
    c64_font1 = new QFont("C64 Pro");
    c64_font1->setPixelSize(8);
    c64_font1->setStyleStrategy(QFont::PreferAntialias);
    c64_font1->setBold(false);
    c64_font1->setKerning(true);

    ui->Diskname->setFont(*c64_font1);
    ui->DIskID->setFont(*c64_font1);

    ui->Filename->setText(tr("Neue_Disk"));
    ui->Diskname->setText(tr("EMU64 DISK"));
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
