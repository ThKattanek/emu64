#ifndef NEW_ROMSET_WINDOW_H
#define NEW_ROMSET_WINDOW_H

#include <QDialog>

namespace Ui {
class NewRomSetWindow;
}

class NewRomSetWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewRomSetWindow(QWidget *parent = 0);
    ~NewRomSetWindow();

private:
    Ui::NewRomSetWindow *ui;
};

#endif // NEW_ROMSET_WINDOW_H
