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
// Letzte Änderung am 18.06.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <cstdio>
#include <QDebug>

#include "./widget_memory_zeile.h"
#include "./ui_widget_memory_zeile.h"

WidgetMemoryZeile::WidgetMemoryZeile(QFont *font1, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetMemoryZeile)
{
    ui->setupUi(this);

    ui->AdresseOut->setFont(*font1);
    ui->EditValue_0->setFont(*font1);
    ui->EditValue_1->setFont(*font1);
    ui->EditValue_2->setFont(*font1);
    ui->EditValue_3->setFont(*font1);
    ui->EditValue_4->setFont(*font1);
    ui->EditValue_5->setFont(*font1);
    ui->EditValue_6->setFont(*font1);
    ui->EditValue_7->setFont(*font1);
    ui->EditValue_8->setFont(*font1);
    ui->EditValue_9->setFont(*font1);
    ui->EditValue_10->setFont(*font1);
    ui->EditValue_11->setFont(*font1);
    ui->EditValue_12->setFont(*font1);
    ui->EditValue_13->setFont(*font1);
    ui->EditValue_14->setFont(*font1);
    ui->EditValue_15->setFont(*font1);
    ui->ReadOut->setFont(*font1);
    ui->WriteOut->setFont(*font1);

    int font_height = ui->AdresseOut->fontMetrics().height();
    int font_weigth = ui->AdresseOut->fontMetrics().averageCharWidth();

    this->setFixedHeight(font_height + 8);

    ui->AdresseOut->setMinimumWidth(5 * font_weigth + 4);
    ui->EditValue_0->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_1->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_2->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_3->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_4->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_5->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_6->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_7->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_8->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_9->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_10->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_11->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_12->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_13->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_14->setMinimumWidth(2 * font_weigth + 4);
    ui->EditValue_15->setMinimumWidth(2 * font_weigth + 4);
    ui->ReadOut->setMinimumWidth(10 * font_weigth + 4);
    ui->WriteOut->setMinimumWidth(10 * font_weigth + 4);

    ui->AdresseOut->setMaximumWidth(5 * font_weigth + 4);
    ui->EditValue_0->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_1->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_2->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_3->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_4->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_5->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_6->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_7->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_8->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_9->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_10->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_11->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_12->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_13->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_14->setMaximumWidth(2 * font_weigth + 4);
    ui->EditValue_15->setMaximumWidth(2 * font_weigth + 4);
    ui->ReadOut->setMaximumWidth(9 * font_weigth + 4);
    ui->WriteOut->setMaximumWidth(9 * font_weigth + 4);

    int w = (5 * font_weigth + 4) + (16*(2 * font_weigth + 4)) + (2*(10 * font_weigth + 4));

    this->setFixedWidth(w);

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
}

void WidgetMemoryZeile::SelectColumb(uint8_t columb)
{
    switch(columb)
    {
    case 0:
        ui->EditValue_0->selectAll();
        ui->EditValue_0->setFocus();
        break;
    case 1:
        ui->EditValue_1->selectAll();
        ui->EditValue_1->setFocus();
        break;
    case 2:
        ui->EditValue_2->selectAll();
        ui->EditValue_2->setFocus();
        break;
    case 3:
        ui->EditValue_3->selectAll();
        ui->EditValue_3->setFocus();
        break;
    case 4:
        ui->EditValue_4->selectAll();
        ui->EditValue_4->setFocus();
        break;
    case 5:
        ui->EditValue_5->selectAll();
        ui->EditValue_5->setFocus();
        break;
    case 6:
        ui->EditValue_6->selectAll();
        ui->EditValue_6->setFocus();
        break;
    case 7:
        ui->EditValue_7->selectAll();
        ui->EditValue_7->setFocus();
        break;
    case 8:
        ui->EditValue_8->selectAll();
        ui->EditValue_8->setFocus();
        break;
    case 9:
        ui->EditValue_9->selectAll();
        ui->EditValue_9->setFocus();
        break;
    case 10:
        ui->EditValue_10->selectAll();
        ui->EditValue_10->setFocus();
        break;
    case 11:
        ui->EditValue_11->selectAll();
        ui->EditValue_11->setFocus();
        break;
    case 12:
        ui->EditValue_12->selectAll();
        ui->EditValue_12->setFocus();
        break;
    case 13:
        ui->EditValue_13->selectAll();
        ui->EditValue_13->setFocus();
        break;
    case 14:
        ui->EditValue_14->selectAll();
        ui->EditValue_14->setFocus();
        break;
    case 15:
        ui->EditValue_15->selectAll();
        ui->EditValue_15->setFocus();
        break;
    }
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
