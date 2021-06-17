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
	~ColorButton();

	void SetDefaultColor(QColor color);
	void SetColor(QColor color);
	void ResetColor();
	QColor GetColor();

	void SetColorNumber(int color_number);

	void Select();
	void DisSelect();

signals:
	void Clicked(int color_number);

protected:
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;

private:
	Ui::ColorButton *ui;

	QColor default_color;
	QColor color;
	int color_number;

	bool is_selected;
};

#endif // COLOR_BUTTON_H
