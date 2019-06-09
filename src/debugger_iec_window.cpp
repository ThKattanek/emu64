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
// Letzte Änderung am 09.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "debugger_iec_window.h"
#include "ui_debugger_iec_window.h"

#include <QFileDialog>
#include <QMessageBox>

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

bool DebuggerIECWindow::GetSaveFileName(QWidget *parent, QString caption, QString filter, QString *file_name, QString *file_extension)
{
   if (file_name == nullptr)      // "parent" is allowed to be NULL!
      return false;

   QFileDialog saveDialog(parent);
   saveDialog.setWindowTitle(caption);
   saveDialog.setAcceptMode(QFileDialog::AcceptSave);
   saveDialog.setConfirmOverwrite(false);
   saveDialog.setFilter(filter);
   saveDialog.selectFile(*file_name);
   saveDialog.setOptions(QFileDialog::DontUseNativeDialog);

   *file_name = "";

   if (!saveDialog.exec())
      return false;      // User pressed "Cancel"

   QStringList fileList = saveDialog.selectedFiles();
   if (fileList.count() != 1)
      return false;      // Should not happen, just to be sure

   QString tmpFileName = fileList.at(0);
   QString extension;

   QFileInfo fileInfo(tmpFileName);
   if (fileInfo.suffix().isEmpty()) {
      // Add the suffix selected by the user

      extension = saveDialog.selectedFilter();
      extension = extension.right(extension.size() - extension.indexOf("*.") - 2);
      extension = extension.left(extension.indexOf(")"));
      extension = extension.simplified();

      // If the filter specifies more than one extension, choose the first one
      if (extension.indexOf(" ") != -1)
         extension = extension.left(extension.indexOf(" "));

      tmpFileName = tmpFileName + QString(".") + extension;
      fileInfo.setFile(tmpFileName);
   }

   // Does the file already exist?
   /*
   if (QFile::exists(tmpFileName)) {

       extension = saveDialog.selectedFilter();
       extension = extension.right(extension.size() - extension.indexOf("*.") - 2);
       extension = extension.left(extension.indexOf(")"));
       extension = extension.simplified();

      int result = QMessageBox::question(parent, QObject::trUtf8("Überschreiben?"),
         QObject::trUtf8("Soll die Datei \"%1\" überschrieben werden?").arg(fileInfo.fileName()),
         QMessageBox::Yes,
         QMessageBox::No | QMessageBox::Default,
         QMessageBox::Cancel | QMessageBox::Escape);
      if (result == QMessageBox::Cancel)
         return false;
      else if (result == QMessageBox::No) {
         // Next chance for the user to select a filename
         if (!getSaveFileName(parent, caption, filter, &tmpFileName, &extension))
            // User decided to cancel, exit function here
            return false;
      // User clicked "Yes", so process the execution
      fileInfo.setFile(tmpFileName);
      }
   }
   */

   *file_name = tmpFileName;
   *file_extension = extension;
   return true;
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

    //QString filename = QFileDialog::getSaveFileName(0,trUtf8("Als VCD speichern"),QDir::homePath(),trUtf8("Value Change Dump ") + "(*.vcd);;" + trUtf8("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);


    if(!GetSaveFileName(this,trUtf8("Als VCD speichern"),trUtf8("Value Change Dump ") + "(*.vcd);;" + trUtf8("Alle Dateien ") + "(*.*)",&filename,&fileext))
        return;


    if(filename != "")
    {
        if(c64->StartIECDump(filename.toLatin1().data()))
        {
            ui->StopExport->setEnabled(true);
            ui->StartExport->setEnabled(false);
        }
        else
        {
            // Fehler beim erstellen des IEC Dumps
            QMessageBox::critical(this,trUtf8("Fehler beim speichern !"),trUtf8("Leider konnte der IEC Dump nicht gestartet werden."));
        }
    }
}
