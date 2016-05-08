//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: debugger_iec_window.cpp               //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 08.05.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "debugger_iec_window.h"
#include "ui_debugger_iec_window.h"

#include <QFileDialog>

DebuggerIECWindow::DebuggerIECWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebuggerIECWindow),
    c64(NULL)
{
    ui->setupUi(this);

    iOff = new QIcon(":/grafik/green_led_off.png");
    iOn = new QIcon(":/grafik/green_led_on.png");

    ui->ExportVCD->setEnabled(true);
    ui->StopExport->setEnabled(false);
}

DebuggerIECWindow::~DebuggerIECWindow()
{
    delete ui;
}

void DebuggerIECWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void DebuggerIECWindow::SetC64Pointer(C64Class *c64)
{
    this->c64 = c64;
}

void DebuggerIECWindow::UpdateSignals()
{
    if(c64 == NULL) return;

    IEC_STRUCT iec;

    c64->GetIECStatus(&iec);

    if(iec.DATA_OUT) ui->DataOut->setIcon(*iOn);
    else ui->DataOut->setIcon(*iOff);

    if(iec.DATA_IN) ui->DataIn->setIcon(*iOn);
    else ui->DataIn->setIcon(*iOff);

    if(iec.CLOCK_OUT) ui->ClkOut->setIcon(*iOn);
    else ui->ClkOut->setIcon(*iOff);

    if(iec.CLOCK_IN) ui->ClkIn->setIcon(*iOn);
    else ui->ClkIn->setIcon(*iOff);

    if(iec.ATN_OUT) ui->AtnOut->setIcon(*iOn);
    else ui->AtnOut->setIcon(*iOff);
}

void DebuggerIECWindow::on_ExportVCD_clicked()
{
    if(c64 == NULL) return;

    QString filename = QFileDialog::getSaveFileName(this,trUtf8("Als VCD speichern"),"",trUtf8("Value Change Dump (*.vcd)") + "(*.vcd);;" + trUtf8("Alle Dateien") + "(*.*)");
    if(filename != "")
    {
        if(c64->StartIECDump(filename.toLatin1().data()))
        {
            ui->StopExport->setEnabled(true);
            ui->ExportVCD->setEnabled(false);
        }
    }
}

void DebuggerIECWindow::on_StopExport_clicked()
{
    c64->StopIECDump();

    ui->ExportVCD->setEnabled(true);
    ui->StopExport->setEnabled(false);
}
