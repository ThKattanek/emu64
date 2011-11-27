//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: widget_d64_file.cpp                   //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 13.10.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include "widget_d64_file.h"
#include "ui_widget_d64_file.h"

WidgetD64File::WidgetD64File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetD64File)
{
    ui->setupUi(this);

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/emu64.ttf");

    QFont font("Emu64 D64 Directory",16);

    green_led = new QIcon(":/grafik/GreenLED_On.png");
    yellow_led = new QIcon(":/grafik/YellowLED_On.png");
    red_led = new QIcon(":/grafik/RedLED_On.png");

    ui->FileName->setFont(font);
    ui->Spur->setFont(font);
    ui->Sektor->setFont(font);
    ui->Adresse->setFont(font);
    ui->Size->setFont(font);
    ui->Typ->setFont(font);
}

WidgetD64File::~WidgetD64File()
{
    delete ui;
}

void WidgetD64File::SetLabels(QString filename, QString spur, QString sektor, QString adresse, QString size, QString typ)
{
    ui->FileName->setText(filename);
    ui->Spur->setText(spur);
    ui->Sektor->setText(sektor);
    ui->Adresse->setText(adresse);
    ui->Size->setText(size);
    ui->Typ->setText(typ);
}

void WidgetD64File::SetRMode(int mode)
{
    switch(mode)
    {
    case 0:
        ui->RunMode->setIcon(*green_led);
        break;
    case 1:
        ui->RunMode->setIcon(*yellow_led);
        break;
    case 2:
        ui->RunMode->setIcon(*red_led);
        break;
    }
}
