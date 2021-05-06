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

private:
	Ui::ColorButton *ui;
};

#endif // COLOR_BUTTON_H
