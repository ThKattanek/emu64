//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: c64_keyboard_window.h                 //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 10.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef C64_KEYBOARD_WINDOW_H
#define C64_KEYBOARD_WINDOW_H

#include <cstdint>

#include <QDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QSettings>
#include <QTimer>

#include "./c64_class.h"

namespace Ui {
    class C64KeyboardWindow;
}

class C64KeyboardWindow : public QDialog
{
    Q_OBJECT

public:
    explicit C64KeyboardWindow(QWidget *parent = nullptr, QSettings *ini = nullptr, C64Class *c64 = nullptr);
    ~C64KeyboardWindow(void);
    void RetranslateUi(void);
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    bool recording;
    uint8_t *key_matrix_to_port_a;
    uint8_t *key_matrix_to_port_b;

public slots:
    void TimerEvent();

private:

    QSettings *ini;
    Ui::C64KeyboardWindow *ui;
    QTimer *timer;

    uint8_t current_y_key_old;
    uint8_t current_y_key;
    uint8_t current_x_key_old;
    uint8_t current_x_key;

    QPoint mouse_offset;

    int ws_x;
    int ws_y;
    int cm_x;
    int cm_y;

    float_t scaling_x;
    float_t scaling_y;

    bool blink_flip;

    uint8_t *key_trans;

    bool rec_key_press;
    uint8_t rec_key_curr_x;
    uint8_t rec_key_curr_y;

    int rec_time_out;
    bool is_one_showed;

    C64Class *c64;
};

#endif // C64_KEYBOARD_WINDOW_H
