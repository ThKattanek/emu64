//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: floppy_window.cpp                     //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 18.04.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include <QFontDatabase>

#include "floppy_window.h"
#include "ui_floppy_window.h"

FloppyWindow::FloppyWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FloppyWindow)
{
    ui->setupUi(this);

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/emu64.ttf");

    //ui->treeWidget->setFont(QFont("Emu64 D64 Directory",16));

    QFont font("Emu64 D64 Directory",16);
    font.setBold(false);
    ui->label->setFont(font);


    ui->treeWidget->setColumnCount(3);
    QStringList headers;
    headers << tr("Dateien") << tr("Göse") << tr("Position");
    ui->treeWidget->setHeaderLabels(headers);
}

FloppyWindow::~FloppyWindow()
{
    delete ui;
}

void FloppyWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}
