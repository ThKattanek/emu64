#include "color_button.h"
#include "ui_color_button.h"

#include <QPainter>
#include <QPainterPath>

ColorButton::ColorButton(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ColorButton)
{
	ui->setupUi(this);

	this->setMinimumWidth(50);
	this->setMinimumHeight(50);

	color_number = 0;
}

void ColorButton::paintEvent(QPaintEvent *event)
{
	QPainter p;
	p.begin(this);

	p.setRenderHint(QPainter::Antialiasing);

	p.drawRoundedRect(0,0,this->width()-1, this->height()-1, 10, 10);

	QPainterPath path;
	path.addRoundedRect(QRect(5,5,this->width()-11, this->height()-11),5,5);
	QPen pen(color, 1);
	p.setPen(pen);
	p.fillPath(path, color);
	p.drawPath(path);

	QFont font;
	font.setPixelSize(24);
	font.setBold(true);

	QPainterPath path1;
	path1.addText(6,26, font, QString::number(color_number));

	QPen pen1(Qt::white,1.4);
	p.setPen(pen1);
	p.setBrush(Qt::black);
	p.drawPath(path1);

	p.end();
}

ColorButton::~ColorButton()
{
	delete ui;
}

void ColorButton::SetColor(QColor color)
{
	this->color = color;
}

QColor ColorButton::GetColor()
{
	return color;
}

void ColorButton::SetColorNumber(int color_number)
{
	this->color_number = color_number;
}
