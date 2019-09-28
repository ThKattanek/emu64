//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tape_new_window.cpp                   //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 28.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QDesktopWidget>

#include "tape_new_window.h"
#include "ui_tape_new_window.h"

TapeNewWindow::TapeNewWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TapeNewWindow)
{
    ui->setupUi(this);

    // Center Window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
}

TapeNewWindow::~TapeNewWindow()
{
    delete ui;
}

QString TapeNewWindow::GetFilename()
{
    return ui->TapeName->text();
}
