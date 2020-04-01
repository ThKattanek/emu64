//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_sr_edit.h                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 01.04.2020                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef WIDGET_SR_EDIT_H
#define WIDGET_SR_EDIT_H

#include <QWidget>

namespace Ui {
    class WidgetSREdit;
}

class WidgetSREdit : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSREdit(QWidget *parent = 0);
    ~WidgetSREdit();
    void RetranslateUi();
    void SetValue(uint8_t value);

private:
    void CalcValue(void);

    Ui::WidgetSREdit *ui;
    QIcon *iOff;
    QIcon *iOn;
    bool bit0,bit1,bit2,bit3,bit4,bit5,bit6,bit7;


 signals:
    void ChangeValue(uint8_t value);

private slots:
    void on_Bit0_clicked();
    void on_Bit1_clicked();
    void on_Bit2_clicked();
    void on_Bit3_clicked();
    void on_Bit4_clicked();
    void on_Bit5_clicked();
    void on_Bit6_clicked();
    void on_Bit7_clicked();
};

#endif // WIDGET_SR_EDIT_H
