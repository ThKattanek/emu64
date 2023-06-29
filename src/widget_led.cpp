//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_led.cpp		                //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 01.06.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "widget_led.h"
#include "ui_widget_led.h"

#include <QPainter>

WidgetLED::WidgetLED(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetLED)
{
	ui->setupUi(this);

	brightness = 0.0f;
	color_on.setBlue(255);
}

WidgetLED::~WidgetLED()
{
	delete ui;
}

void WidgetLED::SetColorOn(QColor color)
{
	color_on = color;
}

void WidgetLED::SetColorOff(QColor color)
{
	color_off = color;
}

void WidgetLED::SetBrightness(float brightness)
{
	this->brightness = brightness;
	this->update();
}

void WidgetLED::paintEvent(QPaintEvent *event)
{
	(void)event;

	int16_t new_r = color_off.red() + (color_on.red() - color_off.red()) * brightness * 0.6f;
	int16_t new_g = color_off.green() + (color_on.green() - color_off.green()) * brightness * 0.6f;
	int16_t new_b = color_off.blue() + (color_on.blue() - color_off.blue()) * brightness * 0.6f;
	QColor new_color_1(new_r, new_g, new_b);

	new_r = color_off.red() + (color_on.red() - color_off.red()) * brightness * 0.8f;
	new_g = color_off.green() + (color_on.green() - color_off.green()) * brightness * 0.8f;
	new_b = color_off.blue() + (color_on.blue() - color_off.blue()) * brightness * 0.8f;
	QColor new_color_2(new_r, new_g, new_b);

	new_r = color_off.red() + (color_on.red() - color_off.red()) * brightness;
	new_g = color_off.green() + (color_on.green() - color_off.green()) * brightness;
	new_b = color_off.blue() + (color_on.blue() - color_off.blue()) * brightness;
	QColor new_color_3(new_r, new_g, new_b);

	QPainter p;
	p.begin(this);
		p.fillRect(0,0,this->width(),this->height(),new_color_1);
		p.fillRect(1,1,this->width()-2,this->height()-2,new_color_2);
		p.fillRect(2,2,this->width()-4,this->height()-4,new_color_3);
	p.end();
}
