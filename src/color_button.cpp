#include "color_button.h"
#include "ui_color_button.h"

#include <QPainter>

ColorButton::ColorButton(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ColorButton)
{
	ui->setupUi(this);

	this->setMinimumWidth(50);
	this->setMinimumHeight(50);
}

void ColorButton::paintEvent(QPaintEvent *event)
{
	QPainter p;
	p.begin(this);

	p.drawRoundRect(0,0,this->width()-1, this->height()-1, 5, 5);

	p.end();
}

ColorButton::~ColorButton()
{
	delete ui;
}
