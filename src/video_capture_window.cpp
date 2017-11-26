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
// Letzte Änderung am 05.10.2017                //
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

    ui->CaptureStop->setEnabled(false);
    ui->AudioBitrate->setMinimum(16);
    ui->AudioBitrate->setMaximum(1024);
    ui->AudioBitrate->setValue(128);

    ui->VideoBitrate->setMinimum(100);
    ui->VideoBitrate->setMaximum(16000);
    ui->VideoBitrate->setValue(4000);

    counter_pause = 0;
    isPause = false;

    timer1 = new QTimer(this);
    timer1->setInterval(10);
    connect(timer1,SIGNAL(timeout()),this,SLOT(OnTimer1()));
}

VideoCaptureWindow::~VideoCaptureWindow()
{
    delete timer1;
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

    if(c64->StartVideoRecord(filename.toLatin1().data(),ui->AudioBitrate->value()*1000,ui->VideoBitrate->value()*1000))
    {
        ui->CaptureStart->setEnabled(false);
        ui->CaptureStop->setEnabled(true);
        timer1->start();
    }
}

void VideoCaptureWindow::on_CaptureStop_clicked()
{
    if(c64 == 0) return;

    ui->CaptureStart->setEnabled(true);
    ui->CaptureStop->setEnabled(false);

    c64->StopVideoRecord();
    timer1->stop();
}

void VideoCaptureWindow::on_CapturePause_clicked(bool checked)
{
    if(c64 == 0) return;
    c64->SetPauseVideoRecord(checked);

    isPause = checked;
    if(isPause)
        counter_pause = 0;
    else
        ui->TimeOutput->setVisible(true);
}

void VideoCaptureWindow::OnTimer1()
{
    if(c64 == 0) return;

    if(!isPause)
    {
        int time = c64->GetRecordedFrameCount();
        int hour = time / (50*60*60);
        time %= (50*60*60);
        int minutes = time / (50*60);
        time %= (50*60);
        int seconds = time / 50;
        int frames = time % 50;

        char out_str[16];

        sprintf(out_str,"%.2d:%.2d:%.2d-%.2d",hour,minutes,seconds,frames);
        ui->TimeOutput->setText(out_str);
    }
    else
    {
        int time = c64->GetRecordedFrameCount();
        int hour = time / (50*60*60);
        time %= (50*60*60);
        int minutes = time / (50*60);
        time %= (50*60);
        int seconds = time / 50;
        int frames = time % 50;

        char out_str[16];

        sprintf(out_str,"%.2d:%.2d:%.2d-%.2d",hour,minutes,seconds,frames);
        ui->TimeOutput->setText(out_str);


        counter_pause++;
        if(counter_pause == 25)
        {
            counter_pause = 0;
            if(ui->TimeOutput->isVisible())
                ui->TimeOutput->setVisible(false);
            else
                ui->TimeOutput->setVisible(true);
        }
    }
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
