#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

#include <QWidget>

namespace Ui {
class ColorButton;
}

class ColorButton : public QWidget
{
	Q_OBJECT

public:
	explicit ColorButton(QWidget *parent = nullptr);
	void paintEvent(QPaintEvent *event);
	~ColorButton();

	void SetColor(QColor color);
	QColor GetColor();

	void SetColorNumber(int color_number);

private:
	Ui::ColorButton *ui;

	QColor color;
	int color_number;
};

#endif // COLOR_BUTTON_H
