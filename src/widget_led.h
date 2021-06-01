//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_led.h			                //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 01.06.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef WIDGET_LED_H
#define WIDGET_LED_H

#include <QWidget>

namespace Ui {
class WidgetLED;
}

class WidgetLED : public QWidget
{
	Q_OBJECT

public:
	explicit WidgetLED(QWidget *parent = nullptr);
	~WidgetLED();

	void SetColorOn(QColor color);
	void SetColorOff(QColor color);
	void SetBrightness(float brightness);

protected:
	void paintEvent(QPaintEvent *event);

private:
	Ui::WidgetLED *ui;

	QColor color_on;
	QColor color_off;

	float brightness;
};

#endif // WIDGET_LED_H
