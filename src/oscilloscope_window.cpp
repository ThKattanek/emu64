#include "oscilloscope_window.h"
#include "ui_oscilloscope_window.h"

#include <QDebug>

OscilloscopeWindow::OscilloscopeWindow(QWidget *parent, int samplerate) :
    QDialog(parent),
    ui(new Ui::OscilloscopeWindow)
{
    ui->setupUi(this);

    is_hide = true;

    ui->oscilloscope0->setFixedSize(10*20, 8*20);
    ui->oscilloscope0->SetSamplerate(samplerate);
    ui->oscilloscope0->SetVerticalPosition(0.5f);
    ui->oscilloscope0->SetAmplifire(2.0f);
    ui->oscilloscope0->SetTriggerTyp(RISING_EDGE);
    ui->oscilloscope0->SetTriggerLevel(0);
    ui->oscilloscope0->SetRasterColor(QColor(40,40,40));
    ui->oscilloscope0->SetLineColor(QColor(0,255,0));

    ui->oscilloscope1->setFixedSize(10*20, 8*20);
    ui->oscilloscope1->SetSamplerate(samplerate);
    ui->oscilloscope1->SetVerticalPosition(0.5f);
    ui->oscilloscope1->SetAmplifire(2.0f);
    ui->oscilloscope1->SetTriggerTyp(RISING_EDGE);
    ui->oscilloscope1->SetTriggerLevel(0);
    ui->oscilloscope1->SetRasterColor(QColor(40,40,40));
    ui->oscilloscope1->SetLineColor(QColor(0,255,0));

    ui->oscilloscope2->setFixedSize(10*20, 8*20);
    ui->oscilloscope2->SetSamplerate(samplerate);
    ui->oscilloscope2->SetVerticalPosition(0.5f);
    ui->oscilloscope2->SetAmplifire(2.0f);
    ui->oscilloscope2->SetTriggerTyp(RISING_EDGE);
    ui->oscilloscope2->SetTriggerLevel(0);
    ui->oscilloscope2->SetRasterColor(QColor(40,40,40));
    ui->oscilloscope2->SetLineColor(QColor(0,255,0));
}

OscilloscopeWindow::~OscilloscopeWindow()
{
    delete ui;
}

void OscilloscopeWindow::showEvent(QShowEvent *event)
{
    is_hide = false;
}

void OscilloscopeWindow::hideEvent(QHideEvent *event)
{
    is_hide = true;
}

void OscilloscopeWindow::AudioStreamEvent(uint8_t *stream0, uint8_t *stream1,uint8_t *stream2, int length)
{
    if(is_hide) return;
    ui->oscilloscope0->NextAudioData(stream0, length);
    ui->oscilloscope1->NextAudioData(stream1, length);
    ui->oscilloscope2->NextAudioData(stream2, length);
}
