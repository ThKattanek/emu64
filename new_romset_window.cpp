#include "new_romset_window.h"
#include "ui_new_romset_window.h"

NewRomSetWindow::NewRomSetWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewRomSetWindow)
{
    ui->setupUi(this);
}

NewRomSetWindow::~NewRomSetWindow()
{
    delete ui;
}
