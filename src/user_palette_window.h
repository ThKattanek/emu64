#ifndef USER_PALETTE_WINDOW_H
#define USER_PALETTE_WINDOW_H

#include <QDialog>

namespace Ui {
class UserPaletteWindow;
}

class UserPaletteWindow : public QDialog
{
	Q_OBJECT

public:
	explicit UserPaletteWindow(QWidget *parent = nullptr);
	~UserPaletteWindow();

private slots:
	void on_red_slider_valueChanged(int value);

	void on_green_slider_valueChanged(int value);

	void on_blue_slider_valueChanged(int value);

private:
	Ui::UserPaletteWindow *ui;
};

#endif // USER_PALETTE_WINDOW_H
