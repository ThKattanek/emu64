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
// Letzte Änderung am 16.08.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./video_capture_window.h"
#include "./ui_video_capture_window.h"

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
    if(c64 == nullptr) return;

    QString filename;
    QString fileext;

    QStringList filters;
    filters << tr("MPEG-4 (*.mp4)")
            << tr("MKV (Matroschka) (*.mkv)")
            << tr("AVI (*.avi)")
            << tr("QuickTime (*.mov)")
            << tr("Alle Dateien (*.*)");

    if(!CustomSaveFileDialog::GetSaveFileName(this,tr("Video Aufzeichen ..."), filters, &filename, &fileext))
    {
        return;
    }

    if(c64->StartVideoRecord(filename.toUtf8(),ui->AudioBitrate->value() * 1000, ui->VideoBitrate->value()*1000))
    {
        ui->CaptureStart->setEnabled(false);
        ui->CaptureStop->setEnabled(true);
        timer1->start();
    }
}

void VideoCaptureWindow::on_CaptureStop_clicked()
{
    if(c64 == nullptr) return;

    ui->CaptureStart->setEnabled(true);
    ui->CaptureStop->setEnabled(false);

    c64->StopVideoRecord();
    timer1->stop();
}

void VideoCaptureWindow::on_CapturePause_clicked(bool checked)
{
    if(c64 == nullptr) return;
    c64->SetPauseVideoRecord(checked);

    isPause = checked;
    if(isPause)
        counter_pause = 0;
    else
        ui->TimeOutput->setVisible(true);
}

void VideoCaptureWindow::OnTimer1()
{
    if(c64 == nullptr) return;

    if(!isPause)
    {
        uint32_t time = static_cast<uint32_t>(c64->GetRecordedFrameCount());
        uint32_t  hour = time / (50 * 60 * 60);
        time %= (50 * 60 * 60);
        uint32_t  minutes = time / (50 * 60);
        time %= (50 * 60);
        uint32_t  seconds = time / 50;
        uint32_t  frames = time % 50;

        char out_str[16];

        sprintf(out_str,"%02d:%02d:%02d-%02d",hour,minutes,seconds,frames);
        ui->TimeOutput->setText(out_str);
    }
    else
    {
        uint32_t time = static_cast<uint32_t>(c64->GetRecordedFrameCount());
        uint32_t hour = time / (50 * 60 * 60);
        time %= (50 * 60 * 60);
        uint32_t minutes = time / (50 * 60);
        time %= (50 * 60);
        uint32_t seconds = time / 50;
        uint32_t frames = time % 50;

        char out_str[16];

        sprintf(out_str,"%02d:%02d:%02d-%02d",hour,minutes,seconds,frames);
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
