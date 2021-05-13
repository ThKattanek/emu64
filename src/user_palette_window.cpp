#include "user_palette_window.h"
#include "ui_user_palette_window.h"

UserPaletteWindow::UserPaletteWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::UserPaletteWindow)
{
	ui->setupUi(this);

	on_red_slider_valueChanged(0);
	on_blue_slider_valueChanged(0);
	on_green_slider_valueChanged(0);

	ui->red_slider->setMaximum(255);
	ui->green_slider->setMaximum(255);
	ui->blue_slider->setMaximum(255);
}

UserPaletteWindow::~UserPaletteWindow()
{
	delete ui;
}

void UserPaletteWindow::on_red_slider_valueChanged(int value)
{
	ui->red_out->setText(QString::number(value));
}

void UserPaletteWindow::on_green_slider_valueChanged(int value)
{
	ui->green_out->setText(QString::number(value));
}

void UserPaletteWindow::on_blue_slider_valueChanged(int value)
{
	ui->blue_out->setText(QString::number(value));
}
