//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: video_capture_window.cpp              //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 24.09.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "video_capture_window.h"
#include "ui_video_capture_window.h"

VideoCaptureWindow::VideoCaptureWindow(QWidget *parent, C64Class *c64) :
    QDialog(parent),
    ui(new Ui::VideoCaptureWindow)
{
    ui->setupUi(this);
    this->c64 = c64;
}

VideoCaptureWindow::~VideoCaptureWindow()
{
    delete ui;
}

void VideoCaptureWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void VideoCaptureWindow::on_CaptureStart_clicked()
{
    if(c64 == 0) return;

    QString filename;
    QString fileext;
    if(!getSaveFileName(this,trUtf8("Video Aufzeichen ..."),trUtf8("MPEG-4") + "(*.mp4);;" + trUtf8("MKV (Matroschka)") + "(*.mkv);;" + trUtf8("AVI") + "(*.avi);;" + trUtf8("QuickTime") + "(*.mov)",&filename,&fileext))
    {
        return;
    }

    c64->StartVideoRecord(filename.toLatin1().data());
}

void VideoCaptureWindow::on_CaptureStop_clicked()
{
    if(c64 == 0) return;
    c64->StopVideoRecord();
}

void VideoCaptureWindow::on_CapturePause_clicked(bool checked)
{
    if(c64 == 0) return;
    c64->SetPauseVideoRecord(checked);
}

bool VideoCaptureWindow::getSaveFileName(QWidget *parent, QString caption, QString filter, QString *fileName, QString *fileExt)
{
   if (fileName == NULL)      // "parent" is allowed to be NULL!
      return false;

   QFileDialog saveDialog(parent);
   saveDialog.setWindowTitle(caption);
   saveDialog.setAcceptMode(QFileDialog::AcceptSave);
   saveDialog.setConfirmOverwrite(false);
   saveDialog.setFilter(filter);
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

   *fileName = tmpFileName;
   *fileExt = extension;
   return true;
}
