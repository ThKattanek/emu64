#ifndef FLOPPY_NEW_D64_WINDOW_H
#define FLOPPY_NEW_D64_WINDOW_H

#include <QDialog>

namespace Ui {
class floppy_new_d64_window;
}

class floppy_new_d64_window : public QDialog
{
    Q_OBJECT

public:
    explicit floppy_new_d64_window(QWidget *parent = 0);
    ~floppy_new_d64_window();

private:
    Ui::floppy_new_d64_window *ui;
};

#endif // FLOPPY_NEW_D64_WINDOW_H
