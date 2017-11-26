//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_memory_zeile.cpp               //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QFontDatabase>
#include <cstdio>

#include "widget_memory_zeile.h"
#include "ui_widget_memory_zeile.h"

WidgetMemoryZeile::WidgetMemoryZeile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetMemoryZeile)
{
    ui->setupUi(this);

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/lucon.ttf");
    QFont font1("Lucida Console",9);

    ui->AdresseOut->setFont(font1);
    ui->EditValue_0->setFont(font1);
    ui->EditValue_1->setFont(font1);
    ui->EditValue_2->setFont(font1);
    ui->EditValue_3->setFont(font1);
    ui->EditValue_4->setFont(font1);
    ui->EditValue_5->setFont(font1);
    ui->EditValue_6->setFont(font1);
    ui->EditValue_7->setFont(font1);
    ui->EditValue_8->setFont(font1);
    ui->EditValue_9->setFont(font1);
    ui->EditValue_10->setFont(font1);
    ui->EditValue_11->setFont(font1);
    ui->EditValue_12->setFont(font1);
    ui->EditValue_13->setFont(font1);
    ui->EditValue_14->setFont(font1);
    ui->EditValue_15->setFont(font1);
    ui->ReadOut->setFont(font1);
    ui->WriteOut->setFont(font1);
}

WidgetMemoryZeile::~WidgetMemoryZeile()
{
    delete ui;
}

void WidgetMemoryZeile::Fill(unsigned short adr, unsigned char *byte_puffer,QString ReadSource,QString WriteDestination)
{
    adresse = adr;

    char str00[10];
    sprintf(str00,"$%4.4X",adresse);
    ui->AdresseOut->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[0]);
    if(!ui->EditValue_0->hasFocus()) ui->EditValue_0->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[1]);
    if(!ui->EditValue_1->hasFocus()) ui->EditValue_1->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[2]);
    if(!ui->EditValue_2->hasFocus()) ui->EditValue_2->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[3]);
    if(!ui->EditValue_3->hasFocus()) ui->EditValue_3->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[4]);
    if(!ui->EditValue_4->hasFocus()) ui->EditValue_4->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[5]);
    if(!ui->EditValue_5->hasFocus()) ui->EditValue_5->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[6]);
    if(!ui->EditValue_6->hasFocus()) ui->EditValue_6->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[7]);
    if(!ui->EditValue_7->hasFocus()) ui->EditValue_7->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[8]);
    if(!ui->EditValue_8->hasFocus()) ui->EditValue_8->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[9]);
    if(!ui->EditValue_9->hasFocus()) ui->EditValue_9->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[10]);
    if(!ui->EditValue_10->hasFocus()) ui->EditValue_10->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[11]);
    if(!ui->EditValue_11->hasFocus()) ui->EditValue_11->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[12]);
    if(!ui->EditValue_12->hasFocus()) ui->EditValue_12->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[13]);
    if(!ui->EditValue_13->hasFocus()) ui->EditValue_13->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[14]);
    if(!ui->EditValue_14->hasFocus()) ui->EditValue_14->setText(QString(str00));
    sprintf(str00,"%2.2X",byte_puffer[15]);
    if(!ui->EditValue_15->hasFocus()) ui->EditValue_15->setText(QString(str00));

    ui->ReadOut->setText(ReadSource);
    ui->WriteOut->setText(WriteDestination);

    ui->BitLeiste_0->SetValue(byte_puffer[0]);
    ui->BitLeiste_1->SetValue(byte_puffer[1]);
    ui->BitLeiste_2->SetValue(byte_puffer[2]);
    ui->BitLeiste_3->SetValue(byte_puffer[3]);
    ui->BitLeiste_4->SetValue(byte_puffer[4]);
    ui->BitLeiste_5->SetValue(byte_puffer[5]);
    ui->BitLeiste_6->SetValue(byte_puffer[6]);
    ui->BitLeiste_7->SetValue(byte_puffer[7]);
    ui->BitLeiste_8->SetValue(byte_puffer[8]);
    ui->BitLeiste_9->SetValue(byte_puffer[9]);
    ui->BitLeiste_10->SetValue(byte_puffer[10]);
    ui->BitLeiste_11->SetValue(byte_puffer[11]);
    ui->BitLeiste_12->SetValue(byte_puffer[12]);
    ui->BitLeiste_13->SetValue(byte_puffer[13]);
    ui->BitLeiste_14->SetValue(byte_puffer[14]);
    ui->BitLeiste_15->SetValue(byte_puffer[15]);
}

bool WidgetMemoryZeile::ConvHex(QString str, unsigned char *value)
{
    bool ok;
    QString in_str = str;
    unsigned short v;

    in_str.insert(0,"0x");
    v = in_str.toUShort(&ok,0);
    if(!ok)
    {
        return false;
    }
    *value = (unsigned char)v;
    return true;
}

void WidgetMemoryZeile::CheckEdit(QLineEdit *edit, unsigned short adresse)
{
    unsigned char value;
    char str00[3];

    if(ConvHex(edit->text(),&value))
    {
        sprintf(str00,"%2.2X",value);
        edit->setText(QString(str00));
        emit ChangeValue(adresse,value);
    }
    else edit->setText("00");
}

void WidgetMemoryZeile::on_EditValue_0_editingFinished()
{
    CheckEdit(ui->EditValue_0,adresse + 0);
}

void WidgetMemoryZeile::on_EditValue_1_editingFinished()
{
    CheckEdit(ui->EditValue_1,adresse + 1);
}

void WidgetMemoryZeile::on_EditValue_2_editingFinished()
{
    CheckEdit(ui->EditValue_2,adresse + 2);
}

void WidgetMemoryZeile::on_EditValue_3_editingFinished()
{
    CheckEdit(ui->EditValue_3,adresse + 3);
}

void WidgetMemoryZeile::on_EditValue_4_editingFinished()
{
    CheckEdit(ui->EditValue_4,adresse + 4);
}

void WidgetMemoryZeile::on_EditValue_5_editingFinished()
{
    CheckEdit(ui->EditValue_5,adresse + 5);
}

void WidgetMemoryZeile::on_EditValue_6_editingFinished()
{
    CheckEdit(ui->EditValue_6,adresse + 6);
}

void WidgetMemoryZeile::on_EditValue_7_editingFinished()
{
    CheckEdit(ui->EditValue_7,adresse + 7);
}

void WidgetMemoryZeile::on_EditValue_8_editingFinished()
{
    CheckEdit(ui->EditValue_8,adresse + 8);
}

void WidgetMemoryZeile::on_EditValue_9_editingFinished()
{
    CheckEdit(ui->EditValue_9,adresse + 9);
}

void WidgetMemoryZeile::on_EditValue_10_editingFinished()
{
    CheckEdit(ui->EditValue_10,adresse + 10);
}

void WidgetMemoryZeile::on_EditValue_11_editingFinished()
{
    CheckEdit(ui->EditValue_11,adresse + 11);
}

void WidgetMemoryZeile::on_EditValue_12_editingFinished()
{
    CheckEdit(ui->EditValue_12,adresse + 12);
}

void WidgetMemoryZeile::on_EditValue_13_editingFinished()
{
    CheckEdit(ui->EditValue_13,adresse + 13);
}

void WidgetMemoryZeile::on_EditValue_14_editingFinished()
{
    CheckEdit(ui->EditValue_14,adresse + 14);
}

void WidgetMemoryZeile::on_EditValue_15_editingFinished()
{
    CheckEdit(ui->EditValue_15,adresse + 15);
}

void WidgetMemoryZeile::onNoFocus(void)
{
    if(!ui->AdresseOut->hasFocus()) ui->AdresseOut->setFocus();
}

void WidgetMemoryZeile::EndableBitLeiste(bool status)
{
    ui->BitLeiste_0->EnableBitLeiste(status);
    ui->BitLeiste_1->EnableBitLeiste(status);
    ui->BitLeiste_2->EnableBitLeiste(status);
    ui->BitLeiste_3->EnableBitLeiste(status);
    ui->BitLeiste_4->EnableBitLeiste(status);
    ui->BitLeiste_5->EnableBitLeiste(status);
    ui->BitLeiste_6->EnableBitLeiste(status);
    ui->BitLeiste_7->EnableBitLeiste(status);
    ui->BitLeiste_8->EnableBitLeiste(status);
    ui->BitLeiste_9->EnableBitLeiste(status);
    ui->BitLeiste_10->EnableBitLeiste(status);
    ui->BitLeiste_11->EnableBitLeiste(status);
    ui->BitLeiste_12->EnableBitLeiste(status);
    ui->BitLeiste_13->EnableBitLeiste(status);
    ui->BitLeiste_14->EnableBitLeiste(status);
    ui->BitLeiste_15->EnableBitLeiste(status);
}
