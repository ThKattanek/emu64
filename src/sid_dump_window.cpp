# //////////////////////////////////////////////////
# //                                              //
# // Emu64                                        //
# // von Thorsten Kattanek                        //
# //                                              //
# // #file: sid_dump_window.cpp                   //
# //                                              //
# // Dieser Sourcecode ist Copyright geschützt!   //
# // Geistiges Eigentum von Th.Kattanek           //
# //                                              //
# // Letzte Änderung am 28.09.2019                //
# // www.emu64.de                                 //
# //                                              //
# //////////////////////////////////////////////////

#include <QWidget>

#include "sid_dump_window.h"
#include "ui_sid_dump_window.h"

SidDumpWindow::SidDumpWindow(QWidget *parent, C64Class *c64) :
    QDialog(parent),
    ui(new Ui::SidDumpWindow)
{
    this->c64 = c64;
    ui->setupUi(this);

    ui->SidDumpStart->setEnabled(true);
    ui->SidDumpStop->setEnabled(false);

    timer1 = new QTimer(this);
    timer1->setInterval(10);
    connect(timer1,SIGNAL(timeout()),this,SLOT(OnTimer1()));
}

SidDumpWindow::~SidDumpWindow()
{
    timer1->stop();
    delete timer1;
    delete ui;
}

void SidDumpWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void SidDumpWindow::on_SidDumpStart_clicked()
{
    if(c64 == nullptr) return;

    QString filename;
    QString fileext;

    QStringList filters;
    filters << tr("Emu64 SID Dump (*.sdp)")
            << tr("Alle Dateien (*.*)");

    if(!CustomSaveFileDialog::GetSaveFileName(this,tr("SID Dump Aufzeichen ..."), filters, &filename, &fileext))
    {
        return;
    }

    if(c64->StartSidDump(filename.toLocal8Bit()))
    {
        ui->SidDumpStart->setEnabled(false);
        ui->SidDumpStop->setEnabled(true);
        timer1->start();
    }
}

void SidDumpWindow::on_SidDumpStop_clicked()
{
    c64->StopSidDump();
    ui->SidDumpStart->setEnabled(true);
    ui->SidDumpStop->setEnabled(false);
    timer1->stop();
}

void SidDumpWindow::OnTimer1()
{
    if(c64 == nullptr) return;

    int32_t time = c64->GetSidDumpFrames();
    int32_t  hour = time / (50 * 60 * 60);
    time %= (50 * 60 * 60);
    int32_t  minutes = time / (50 * 60);
    time %= (50 * 60);
    int32_t  seconds = time / 50;
    int32_t  frames = time % 50;

    char out_str[16];

    sprintf(out_str,"%02d:%02d:%02d-%02d",hour,minutes,seconds,frames);
    ui->TimeOutput->setText(out_str);
}
