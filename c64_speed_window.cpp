#include "c64_speed_window.h"
#include "ui_c64_speed_window.h"

C64SpeedWindow::C64SpeedWindow(QWidget *parent, QSettings *_ini) :
    QDialog(parent),
    ui(new Ui::C64SpeedWindow),
    ini(NULL),c64(NULL),PauseStatus(false),isOneShowed(false)
{
    ini = _ini;

    ui->setupUi(this);
    ui->C64Speed->setValue(100);

    ////////// Load from INI ///////////
    if(ini != NULL)
    {
        ini->beginGroup("C64SpeedWindow");
        if(ini->contains("Geometry")) setGeometry(ini->value("Geometry").toRect());
        else ini->setValue("Geometry",geometry());
        ini->endGroup();
    }
    ////////////////////////////////////
}

C64SpeedWindow::~C64SpeedWindow()
{
    ///////// Save to INI /////////
    if(ini != NULL)
    {
        ini->beginGroup("C64SpeedWindow");
        if(isOneShowed) ini->setValue("Geometry",geometry());
        ini->endGroup();
    }
    ///////////////////////////////
    delete ui;
}

void C64SpeedWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();

    SetPauseTooltip();
}

void C64SpeedWindow::SetC64Pointer(C64Class *_c64)
{
    c64 = _c64;
}

void C64SpeedWindow::showEvent(QShowEvent*)
{
    isOneShowed = true;
}

void C64SpeedWindow::on_C64Speed_valueChanged(int arg1)
{
    ui->ScrollC64Speed->setValue(arg1);
    if(c64 == NULL) return;

    c64->SetC64Speed(arg1);
}


void C64SpeedWindow::on_ScrollC64Speed_valueChanged(int value)
{
    ui->C64Speed->setValue(value);
}

void C64SpeedWindow::on_Set5_clicked()
{
    ui->C64Speed->setValue(5);
}

void C64SpeedWindow::on_Set25_clicked()
{
    ui->C64Speed->setValue(25);
}

void C64SpeedWindow::on_Set50_clicked()
{
    ui->C64Speed->setValue(50);
}

void C64SpeedWindow::on_Set100_clicked()
{
    ui->C64Speed->setValue(100);
}

void C64SpeedWindow::on_Set200_clicked()
{
    ui->C64Speed->setValue(200);
}

void C64SpeedWindow::on_Set300_clicked()
{
    ui->C64Speed->setValue(300);
}

void C64SpeedWindow::on_Set400_clicked()
{
    ui->C64Speed->setValue(400);
}

void C64SpeedWindow::on_Set500_clicked()
{
    ui->C64Speed->setValue(500);
}

void C64SpeedWindow::on_Pause_clicked()
{
    if(c64 == NULL) return;

    if(!PauseStatus)
    {
        PauseStatus = true;
        ui->Pause->setText(tr("Weiter"));
        c64->SetDebugMode(true);
    }
    else
    {
        PauseStatus = false;
        ui->Pause->setText(tr("Pause"));
        c64->SetDebugMode(false);
    }
    SetPauseTooltip();
}

void C64SpeedWindow::SetPauseTooltip()
{
    if(PauseStatus)
    {
        ui->Pause->setToolTip(tr("Lässt den C64, mit der eingestellten Geschwindigkeit, weiterlaufen."));
    }
    else
    {
        ui->Pause->setToolTip(tr("Hält den C64 komplett an."));
    }
}
