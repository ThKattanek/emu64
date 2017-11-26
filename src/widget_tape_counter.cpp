//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_tape_counter.cpp               //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 25.09.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "widget_tape_counter.h"
#include "ui_widget_tape_counter.h"

WidgetTapeCounter::WidgetTapeCounter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetTapeCounter)
{
    ui->setupUi(this);

    pix1 = new QPixmap(":/grafik/counter_numbers_24_384.png");
    pix3 = new QPixmap(":/grafik/counter_numbers_24_32_blend.png");
}

WidgetTapeCounter::~WidgetTapeCounter()
{
    delete ui;
}

void WidgetTapeCounter::SetCounter(unsigned int count)
{
   this->count = count;

   int off;

   int fine0 = count%100;

   count /= 100;
   int z0 = count%10;
   count /= 10;
   int z1 = count%10;
   count /= 10;
   int z2 = count%10;

   int y0 = z0*32 + 32 + ((fine0/100.0)*32.0);

   if(z0==9 ) off = fine0/100.0*32.0;
   else off = 0;
   int y1 = z1*32 + 32 + off;

   if(z1==9 && z0==9) off = fine0/100.0*32.0;
   else off = 0;
   int y2 = z2*32 + 32 + off;

   pix2 = pix1->copy(0,y0,24,32);

   // Schatten drüberblenden
   QPainter p0( &pix2 );
   p0.drawPixmap(0,0,24,32,*pix3);
   p0.end();

   ui->ziffer0->setPixmap(pix2);

   pix2 = pix1->copy(0,y1,24,32);

   // Schatten drüberblenden
   QPainter p1( &pix2 );
   p1.drawPixmap(0,0,24,32,*pix3);
   p1.end();

   ui->ziffer1->setPixmap(pix2);

   pix2 = pix1->copy(0,y2,24,32);

   // Schatten drüberblenden
   QPainter p2( &pix2 );
   p2.drawPixmap(0,0,24,32,*pix3);
   p2.end();

   ui->ziffer2->setPixmap(pix2);
}

unsigned int WidgetTapeCounter::GetCounter()
{
    return count;
}
