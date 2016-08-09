#include "floppy_new_d64_window.h"
#include "ui_floppy_new_d64_window.h"

floppy_new_d64_window::floppy_new_d64_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::floppy_new_d64_window)
{
    ui->setupUi(this);
}

floppy_new_d64_window::~floppy_new_d64_window()
{
    delete ui;
}
