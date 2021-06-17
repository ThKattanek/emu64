#include "color_button.h"
#include "ui_color_button.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>

ColorButton::ColorButton(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ColorButton)
{
	ui->setupUi(this);

	this->setMinimumWidth(50);
	this->setMinimumHeight(50);

	color_number = 0;

	is_selected = false;
}

void ColorButton::paintEvent(QPaintEvent *event)
{
	QPainter p;
	p.begin(this);

	p.setRenderHint(QPainter::Antialiasing);

	QPen pen;

	if(is_selected)
	{
		pen.setColor(Qt::red);
		pen.setWidth(2);
	}
	else
	{
		pen.setColor(Qt::black);
		pen.setWidth(1);
	}

	p.setPen(pen);

	p.drawRoundedRect(1,1,this->width()-2, this->height()-2, 10, 10);

	QPainterPath path;
	path.addRoundedRect(QRect(5,5,this->width()-11, this->height()-11),5,5);
	pen.setColor(color);
	pen.setWidth(1);
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

void ColorButton::SetDefaultColor(QColor color)
{
	default_color = color;
}

void ColorButton::SetColor(QColor color)
{
	this->color = color;
}

void ColorButton::ResetColor()
{
	color = default_color;
	update();
}

QColor ColorButton::GetColor()
{
	return color;
}

void ColorButton::SetColorNumber(int color_number)
{
	this->color_number = color_number;
}

void ColorButton::Select()
{
	is_selected = true;
	this->update();
}

void ColorButton::DisSelect()
{
	is_selected = false;
	this->update();
}

void ColorButton::mousePressEvent(QMouseEvent *event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		Select();
		emit Clicked(color_number);
	}
}
