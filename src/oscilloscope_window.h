#ifndef OSCILLOSCOPE_WINDOW_H
#define OSCILLOSCOPE_WINDOW_H

#include <QDialog>

namespace Ui {
class OscilloscopeWindow;
}

class OscilloscopeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit OscilloscopeWindow(QWidget *parent = nullptr, int samplerate=44100);
    ~OscilloscopeWindow();
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);

    /// AudioStream empfangen
    void AudioStreamEvent(uint8_t *stream0, uint8_t *stream1, uint8_t *stream2, int length);

private:
    Ui::OscilloscopeWindow *ui;
    bool is_hide;
};

#endif // OSCILLOSCOPE_WINDOW_H
