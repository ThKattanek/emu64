#ifndef TAPE_WINDOW_H
#define TAPE_WINDOW_H

#include <QDialog>

namespace Ui {
class TapeWindow;
}

class TapeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TapeWindow(QWidget *parent = 0);
    ~TapeWindow();

private:
    Ui::TapeWindow *ui;
};

#endif // TAPE_WINDOW_H
