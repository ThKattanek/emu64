#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

#include <QWidget>
#include <QMouseEvent>

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

	void Select();
	void DisSelect();

signals:
	void Clicked(int color_number);

protected:
	void mousePressEvent(QMouseEvent *event) override;

private:
	Ui::ColorButton *ui;

	QColor color;
	int color_number;

	bool is_selected;
};

#endif // COLOR_BUTTON_H
