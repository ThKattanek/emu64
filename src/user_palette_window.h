#ifndef USER_PALETTE_WINDOW_H
#define USER_PALETTE_WINDOW_H

#include <QDialog>
#include "./color_button.h"

namespace Ui {
class UserPaletteWindow;
}

class UserPaletteWindow : public QDialog
{
	Q_OBJECT

public:
	explicit UserPaletteWindow(QWidget *parent = nullptr);
	~UserPaletteWindow();

	void SetDefaultColor(int color_number, QColor color);
	void SetColor(int color_number, QColor color);
	QColor GetColor(int color_number);

signals:
	void ChangeColor(int color_number, QColor color);

private slots:
	void on_red_slider_valueChanged(int value);
	void on_green_slider_valueChanged(int value);
	void on_blue_slider_valueChanged(int value);
	void on_red_out_valueChanged(int arg1);
	void on_green_out_valueChanged(int arg1);
	void on_blue_out_valueChanged(int arg1);

	void ColorButtonClicked(int color_number);

	void on_SetDefaultColors_clicked();

private:
	Ui::UserPaletteWindow *ui;
	ColorButton *color_buttons[16];

	int current_select_color;
};

#endif // USER_PALETTE_WINDOW_H
