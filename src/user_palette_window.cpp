#include "user_palette_window.h"
#include "ui_user_palette_window.h"

UserPaletteWindow::UserPaletteWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::UserPaletteWindow)
{
	ui->setupUi(this);
}

UserPaletteWindow::~UserPaletteWindow()
{
	delete ui;
}
