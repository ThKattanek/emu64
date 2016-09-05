#include "tape_window.h"
#include "ui_tape_window.h"

TapeWindow::TapeWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TapeWindow)
{
    ui->setupUi(this);
}

TapeWindow::~TapeWindow()
{
    delete ui;
}
