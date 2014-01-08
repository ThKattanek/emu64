#include "capture_window.h"
#include "ui_capture_window.h"

CaptureWindow::CaptureWindow(QWidget *parent, C64Class *c64, QSettings *ini) :
    QDialog(parent),
    ui(new Ui::CaptureWindow)
{
    this->c64 = c64;
    this->ini = ini;

    ui->setupUi(this);
}

CaptureWindow::~CaptureWindow()
{
    delete ui;
}

void CaptureWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}
