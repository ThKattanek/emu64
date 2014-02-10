//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: c64_keyboard_window.h                 //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 29.04.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef C64_KEYBOARD_WINDOW_H
#define C64_KEYBOARD_WINDOW_H

#include <QDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QSettings>
#include <QTimer>

namespace Ui {
    class C64KeyboardWindow;
}

class C64KeyboardWindow : public QDialog
{
    Q_OBJECT

public:
    explicit C64KeyboardWindow(QWidget *parent = 0, QSettings *_ini = 0);
    ~C64KeyboardWindow();
    void RetranslateUi();
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    bool Recording;
    unsigned char* KeyMatrixToPA;
    unsigned char* KeyMatrixToPB;

public slots:
    void timer_event();

private:

    QSettings *ini;
    Ui::C64KeyboardWindow *ui;
    QTimer *timer;

    int AKT_Y_KEY_OLD;
    int AKT_Y_KEY;
    int AKT_X_KEY_OLD;
    int AKT_X_KEY;

    QPoint mouse_offset;

    int ws_x;
    int ws_y;
    int cm_x;
    int cm_y;

    float scaling_x;
    float scaling_y;

    bool blink_flip;

    unsigned char* KeyboardTrans;

    bool RecKeyPress;
    unsigned char RecKeyAktX;
    unsigned char RecKeyAktY;

    bool isOneShowed;
};

#endif // C64_KEYBOARD_WINDOW_H
