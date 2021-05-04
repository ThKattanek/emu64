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

private:
	Ui::UserPaletteWindow *ui;
};

#endif // USER_PALETTE_WINDOW_H
