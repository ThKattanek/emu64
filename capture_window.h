#ifndef CAPTURE_WINDOW_H
#define CAPTURE_WINDOW_H

#include <QDialog>
#include <QSettings>

#include "c64_class.h"

namespace Ui {
class CaptureWindow;
}

class CaptureWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit CaptureWindow(QWidget *parent = 0, C64Class *c64 = NULL, QSettings *ini = NULL);
    ~CaptureWindow();
    void RetranslateUi();

private:
    Ui::CaptureWindow *ui;

    QSettings *ini;
    C64Class *c64;
};

#endif // CAPTURE_WINDOW_H
