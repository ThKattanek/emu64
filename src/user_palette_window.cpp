#include "user_palette_window.h"
#include "ui_user_palette_window.h"

#include <QDebug>

UserPaletteWindow::UserPaletteWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::UserPaletteWindow)
{
	ui->setupUi(this);

	color_buttons[0] = ui->color_button_0;
	color_buttons[1] = ui->color_button_1;
	color_buttons[2] = ui->color_button_2;
	color_buttons[3] = ui->color_button_3;
	color_buttons[4] = ui->color_button_4;
	color_buttons[5] = ui->color_button_5;
	color_buttons[6] = ui->color_button_6;
	color_buttons[7] = ui->color_button_7;
	color_buttons[8] = ui->color_button_8;
	color_buttons[9] = ui->color_button_9;
	color_buttons[10] = ui->color_button_10;
	color_buttons[11] = ui->color_button_11;
	color_buttons[12] = ui->color_button_12;
	color_buttons[13] = ui->color_button_13;
	color_buttons[14] = ui->color_button_14;
	color_buttons[15] = ui->color_button_15;

	on_red_slider_valueChanged(0);
	on_blue_slider_valueChanged(0);
	on_green_slider_valueChanged(0);

	ui->red_slider->setMaximum(255);
	ui->green_slider->setMaximum(255);
	ui->blue_slider->setMaximum(255);

	ui->red_out->setMaximum(255);
	ui->green_out->setMaximum(255);
	ui->blue_out->setMaximum(255);

	for(int i=0; i<16; i++)
	{
		color_buttons[i]->SetColorNumber(i);
		connect(color_buttons[i], SIGNAL(Clicked(int)), this, SLOT(ColorButtonClicked(int)));
	}

	current_select_color = 0;
	color_buttons[current_select_color]->Select();
}

UserPaletteWindow::~UserPaletteWindow()
{
	delete ui;
}

void UserPaletteWindow::SetDefaultColor(int color_number, QColor color)
{
	if(color_number >= 0 && color_number < 16)
		color_buttons[color_number]->SetDefaultColor(color);
}

void UserPaletteWindow::SetColor(int color_number, QColor color)
{
	if(color_number >= 0 && color_number < 16)
	{
		color_buttons[color_number]->SetColor(color);
		if(color_number == current_select_color)
		{
			on_red_slider_valueChanged(color.red());
			on_green_slider_valueChanged(color.green());
			on_blue_slider_valueChanged(color.blue());
		}
	}
}

QColor UserPaletteWindow::GetColor(int color_number)
{
	if(color_number >= 0 && color_number < 16)
	{
		return color_buttons[color_number]->GetColor();
	}

	return Qt::black;
}

void UserPaletteWindow::on_red_slider_valueChanged(int value)
{
	ui->red_out->setValue(value);

	QColor color = color_buttons[current_select_color]->GetColor();
	color.setRed(value);
	color_buttons[current_select_color]->SetColor(color);
	color_buttons[current_select_color]->update();

	emit ChangeColor(current_select_color, color);
}

void UserPaletteWindow::on_green_slider_valueChanged(int value)
{
	ui->green_out->setValue(value);

	QColor color = color_buttons[current_select_color]->GetColor();
	color.setGreen(value);
	color_buttons[current_select_color]->SetColor(color);
	color_buttons[current_select_color]->update();

	emit ChangeColor(current_select_color, color);
}

void UserPaletteWindow::on_blue_slider_valueChanged(int value)
{
	ui->blue_out->setValue(value);

	QColor color = color_buttons[current_select_color]->GetColor();
	color.setBlue(value);
	color_buttons[current_select_color]->SetColor(color);
	color_buttons[current_select_color]->update();

	emit ChangeColor(current_select_color, color);
}

void UserPaletteWindow::on_red_out_valueChanged(int arg1)
{
	ui->red_slider->setValue(arg1);
}

void UserPaletteWindow::on_green_out_valueChanged(int arg1)
{
	ui->green_slider->setValue(arg1);
}

void UserPaletteWindow::on_blue_out_valueChanged(int arg1)
{
	ui->blue_slider->setValue(arg1);
}

void UserPaletteWindow::ColorButtonClicked(int color_number)
{
	current_select_color = color_number;
	for(int i=0; i<16; i++)
	{
		if(i != color_number)
			color_buttons[i]->DisSelect();
	}

	QColor color = color_buttons[color_number]->GetColor();

	on_red_slider_valueChanged(color.red());
	on_green_slider_valueChanged(color.green());
	on_blue_slider_valueChanged(color.blue());

}

void UserPaletteWindow::on_SetDefaultColors_clicked()
{
	for(int i=0; i<16; i++)
	{
		color_buttons[i]->ResetColor();

	}
}
