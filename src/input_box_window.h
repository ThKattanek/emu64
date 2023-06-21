//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: input_box_window.h                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 23.04.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef INPUT_BOX_WINDOW_H
#define INPUT_BOX_WINDOW_H

#include <QDialog>

namespace Ui {
class InputBoxWindow;
}

class InputBoxWindow : public QDialog
{
    Q_OBJECT

public:
    explicit InputBoxWindow(QWidget *parent = nullptr);
    ~InputBoxWindow();

    void setValue(uint64_t newValue);
    uint64_t getValue() const;
    void setRange_min(uint64_t newRange_min);
    void setRange_max(uint64_t newRange_max);

private slots:
    void on_edit_dec_textChanged(const QString &arg1);
    void on_edit_hex_textChanged(const QString &arg1);
    void on_edit_bin_textChanged(const QString &arg1);

private:
    Ui::InputBoxWindow *ui;
    uint64_t value;
    uint64_t range_min;
    uint64_t range_max;
};

#endif // INPUT_BOX_WINDOW_H
