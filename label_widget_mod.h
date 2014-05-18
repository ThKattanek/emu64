//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: label_widget_mod.h                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef LABEL_WIDGET_MOD_H
#define LABEL_WIDGET_MOD_H

#include <QLabel>
#include <QMouseEvent>

class LabelWidgetMod : public QLabel
{
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
public:
    explicit LabelWidgetMod(QWidget *parent = 0);

signals:
    void clicked(LabelWidgetMod* label);

public slots:

private:
    bool pressed;

};

#endif // LABEL_WIDGET_MOD_H
