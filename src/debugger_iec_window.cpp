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
// Letzte Änderung am 13.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./debugger_iec_window.h"
#include "./ui_debugger_iec_window.h"
#include "./custom_save_file_dialog.h"

DebuggerIECWindow::DebuggerIECWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebuggerIECWindow),
    c64(nullptr)
{
    ui->setupUi(this);

    icon_off = new QIcon(":/grafik/green_led_off.png");
    icon_on = new QIcon(":/grafik/green_led_on.png");

    ui->StartExport->setEnabled(true);
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
    if(c64 == nullptr)
        return;

    IEC_STRUCT iec;

    c64->GetIECStatus(&iec);

    if(iec.DATA_OUT) ui->DataOut->setIcon(*icon_on);
    else ui->DataOut->setIcon(*icon_off);

    if(iec.DATA_IN) ui->DataIn->setIcon(*icon_on);
    else ui->DataIn->setIcon(*icon_off);

    if(iec.CLOCK_OUT) ui->ClkOut->setIcon(*icon_on);
    else ui->ClkOut->setIcon(*icon_off);

    if(iec.CLOCK_IN) ui->ClkIn->setIcon(*icon_on);
    else ui->ClkIn->setIcon(*icon_off);

    if(iec.ATN_OUT) ui->AtnOut->setIcon(*icon_on);
    else ui->AtnOut->setIcon(*icon_off);
}

void DebuggerIECWindow::on_StopExport_clicked()
{
    c64->StopIECDump();

    ui->StartExport->setEnabled(true);
    ui->StopExport->setEnabled(false);
}

void DebuggerIECWindow::on_StartExport_clicked()
{
    QString filename;
    QString fileext;

    if(c64 == nullptr)
        return;

    QStringList filters;
    filters << tr("Value Change Dump (*.vcd)")
            << tr("Alle Dateien (*.*)");

    if(!CustomSaveFileDialog::GetSaveFileName(this,tr("Als VCD speichern"), filters, &filename, &fileext))
        return;

    if(filename != "")
    {
        if(c64->StartIECDump(filename.toLocal8Bit()))
        {
            ui->StopExport->setEnabled(true);
            ui->StartExport->setEnabled(false);
        }
        else
        {
            // Fehler beim erstellen des IEC Dumps
            QMessageBox::critical(this,tr("Fehler beim speichern !"),tr("Leider konnte der IEC Dump nicht gestartet werden."));
        }
    }
}
