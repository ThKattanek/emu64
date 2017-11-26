//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_tape_counter.h                 //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 25.09.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef WIDGET_TAPE_COUNTER_H
#define WIDGET_TAPE_COUNTER_H

#include <QWidget>
#include <QPainter>

namespace Ui {
class WidgetTapeCounter;
}

class WidgetTapeCounter : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetTapeCounter(QWidget *parent = 0);
    ~WidgetTapeCounter();

    void SetCounter(unsigned int count);
    unsigned int GetCounter(void);

private:
    Ui::WidgetTapeCounter *ui;

    unsigned int count;

    QPixmap *pix1;
    QPixmap pix2;
    QPixmap *pix3;
};

#endif // WIDGET_TAPE_COUNTER_H
