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
# // Letzte Änderung am 07.08.2019                //
# // www.emu64.de                                 //
# //                                              //
# //////////////////////////////////////////////////

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
    if(c64 == NULL) return;

    QString filename;
    QString fileext;
    if(!getSaveFileName(this,trUtf8("SID Dump Aufzeichen ..."),trUtf8("Emu64 SID Dump") + "(*.sdp)",&filename,&fileext))
    {
        return;
    }

    if(c64->StartSidDump(filename.toLatin1().data()))
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
    if(c64 == 0) return;

    int time = c64->GetSidDumpFrames();
    unsigned  hour = time / (50*60*60);
    time %= (50*60*60);
    unsigned  minutes = time / (50*60);
    time %= (50*60);
    unsigned  seconds = time / 50;
    unsigned  frames = time % 50;

    char out_str[16];

    sprintf(out_str,"%02d:%02d:%02d-%02d",hour,minutes,seconds,frames);
    ui->TimeOutput->setText(out_str);
}

bool SidDumpWindow::getSaveFileName(QWidget *parent, QString caption, QString filter, QString *fileName, QString *fileExt)
{
   if (fileName == NULL)      // "parent" is allowed to be NULL!
      return false;

   QFileDialog saveDialog(parent);
   saveDialog.setWindowTitle(caption);
   saveDialog.setAcceptMode(QFileDialog::AcceptSave);
   saveDialog.setConfirmOverwrite(false);
   //saveDialog.setFilter(filter);
   saveDialog.selectFile(*fileName);
   saveDialog.setOptions(QFileDialog::DontUseNativeDialog);

   *fileName = "";

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

      //extension = saveDialog.selectedFilter();
      extension = extension.right(extension.size() - extension.indexOf("*.") - 2);
      extension = extension.left(extension.indexOf(")"));
      extension = extension.simplified();

      // If the filter specifies more than one extension, choose the first one
      if (extension.indexOf(" ") != -1)
         extension = extension.left(extension.indexOf(" "));

      tmpFileName = tmpFileName + QString(".") + extension;
      fileInfo.setFile(tmpFileName);
   }

   /*
   // Does the file already exist?
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

   *fileName = tmpFileName;
   *fileExt = extension;
   return true;
}
