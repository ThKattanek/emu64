//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_sr_edit.cpp                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 16.02.2013                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "widget_sr_edit.h"
#include "ui_widget_sr_edit.h"

WidgetSREdit::WidgetSREdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSREdit)
{
    ui->setupUi(this);

    iOff = new QIcon(":/grafik/blue_led_off.png");
    iOn = new QIcon(":/grafik/blue_led_on.png");

    /// Alle Bits auf Off stellen ///
    ui->Bit0->setIcon(*iOff);
    ui->Bit1->setIcon(*iOff);
    ui->Bit2->setIcon(*iOff);
    ui->Bit3->setIcon(*iOff);
    ui->Bit4->setIcon(*iOff);
    ui->Bit5->setIcon(*iOff);
    ui->Bit6->setIcon(*iOff);
    ui->Bit7->setIcon(*iOff);

    bit0 = bit1 = bit2 = bit3 = bit4 = bit5 = bit6 = bit7 = false;
}

WidgetSREdit::~WidgetSREdit()
{
    delete ui;
    delete iOff;
    delete iOn;
}

void WidgetSREdit::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void WidgetSREdit::CalcValue(void)
{
    unsigned char value = 0;

    if(bit0) value |= 1;
    if(bit1) value |= 2;
    if(bit2) value |= 4;
    if(bit3) value |= 8;
    if(bit4) value |= 16;
    if(bit5) value |= 32;
    if(bit6) value |= 64;
    if(bit7) value |= 128;

    emit ChangeValue(value);
}

void WidgetSREdit::SetValue(unsigned char value)
{
    if(value & 1)
    {
        bit0 = true;
        ui->Bit0->setIcon(*iOn);
    }
    else
    {
        bit0 = false;
        ui->Bit0->setIcon(*iOff);
    }

    if(value & 2)
    {
        bit1 = true;
        ui->Bit1->setIcon(*iOn);
    }
    else
    {
        bit1 = false;
        ui->Bit1->setIcon(*iOff);
    }

    if(value & 4)
    {
        bit2 = true;
        ui->Bit2->setIcon(*iOn);
    }
    else
    {
        bit2 = false;
        ui->Bit2->setIcon(*iOff);
    }

    if(value & 8)
    {
        bit3 = true;
        ui->Bit3->setIcon(*iOn);
    }
    else
    {
        bit3 = false;
        ui->Bit3->setIcon(*iOff);
    }

    if(value & 16)
    {
        bit4 = true;
        ui->Bit4->setIcon(*iOn);
    }
    else
    {
        bit4 = false;
        ui->Bit4->setIcon(*iOff);
    }

    if(value & 32)
    {
        bit5 = true;
        ui->Bit5->setIcon(*iOn);
    }
    else
    {
        bit5 = false;
        ui->Bit5->setIcon(*iOff);
    }

    if(value & 64)
    {
        bit6 = true;
        ui->Bit6->setIcon(*iOn);
    }
    else
    {
        bit6 = false;
        ui->Bit6->setIcon(*iOff);
    }

    if(value & 128)
    {
        bit7 = true;
        ui->Bit7->setIcon(*iOn);
    }
    else
    {
        bit7 = false;
        ui->Bit7->setIcon(*iOff);
    }
}

void WidgetSREdit::on_Bit0_clicked()
{
    if(bit0)
    {
        bit0 = false;
        ui->Bit0->setIcon(*iOff);
    }
    else
    {
        bit0 = true;
        ui->Bit0->setIcon(*iOn);
    }
    CalcValue();
}

void WidgetSREdit::on_Bit1_clicked()
{
    if(bit1)
    {
        bit1 = false;
        ui->Bit1->setIcon(*iOff);
    }
    else
    {
        bit1 = true;
        ui->Bit1->setIcon(*iOn);
    }
    CalcValue();
}

void WidgetSREdit::on_Bit2_clicked()
{
    if(bit2)
    {
        bit2 = false;
        ui->Bit2->setIcon(*iOff);
    }
    else
    {
        bit2 = true;
        ui->Bit2->setIcon(*iOn);
    }
    CalcValue();
}

void WidgetSREdit::on_Bit3_clicked()
{
    if(bit3)
    {
        bit3 = false;
        ui->Bit3->setIcon(*iOff);
    }
    else
    {
        bit3 = true;
        ui->Bit3->setIcon(*iOn);
    }
    CalcValue();
}

void WidgetSREdit::on_Bit4_clicked()
{
    if(bit4)
    {
        bit4 = false;
        ui->Bit4->setIcon(*iOff);
    }
    else
    {
        bit4 = true;
        ui->Bit4->setIcon(*iOn);
    }
    CalcValue();
}

void WidgetSREdit::on_Bit5_clicked()
{
    bit5 = true;
    ui->Bit5->setIcon(*iOn);
    CalcValue();
}

void WidgetSREdit::on_Bit6_clicked()
{
    if(bit6)
    {
        bit6 = false;
        ui->Bit6->setIcon(*iOff);
    }
    else
    {
        bit6 = true;
        ui->Bit6->setIcon(*iOn);
    }
    CalcValue();
}

void WidgetSREdit::on_Bit7_clicked()
{
    if(bit7)
    {
        bit7 = false;
        ui->Bit7->setIcon(*iOff);
    }
    else
    {
        bit7 = true;
        ui->Bit7->setIcon(*iOn);
    }
    CalcValue();
}
