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
// Letzte Änderung am 30.05.2019                //
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

    bool Recording;
    uint8_t *KeyMatrixToPA;
    uint8_t *KeyMatrixToPB;

public slots:
    void timer_event(void);

private:

    QSettings *ini;
    Ui::C64KeyboardWindow *ui;
    QTimer *timer;

    uint8_t AKT_Y_KEY_OLD;
    uint8_t AKT_Y_KEY;
    uint8_t AKT_X_KEY_OLD;
    uint8_t AKT_X_KEY;

    QPoint mouse_offset;

    int ws_x;
    int ws_y;
    int cm_x;
    int cm_y;

    float_t scaling_x;
    float_t scaling_y;

    bool blink_flip;

    uint8_t *KeyboardTrans;

    bool RecKeyPress;
    uint8_t RecKeyAktX;
    uint8_t RecKeyAktY;

    int RecTimeOut;

    bool isOneShowed;

    C64Class *c64;
};

#endif // C64_KEYBOARD_WINDOW_H
