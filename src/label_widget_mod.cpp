//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: label_widget_mod.cpp                  //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "label_widget_mod.h"

LabelWidgetMod::LabelWidgetMod(QWidget *parent) :
    QLabel(parent),
    pressed(false)
{

}

void LabelWidgetMod::mousePressEvent(QMouseEvent*)
{
    pressed = true;
}

void LabelWidgetMod::mouseReleaseEvent(QMouseEvent *event)
{
    if(pressed && rect().contains(event->pos()))
    {
        emit clicked(this);
    }
}
