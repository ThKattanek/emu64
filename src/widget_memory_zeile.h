//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_memory_zeile.h                 //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 14.06.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef WIDGET_MEMORY_ZEILE_H
#define WIDGET_MEMORY_ZEILE_H

#include <QLineEdit>
#include <QFontDatabase>

namespace Ui {
    class WidgetMemoryZeile;
}

class WidgetMemoryZeile : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetMemoryZeile(QFont *font1, QWidget *parent = nullptr);
    ~WidgetMemoryZeile();
    void Fill(unsigned short adr, unsigned char* byte_puffer,QString ReadSource,QString WriteDestination);

private slots:
    void on_EditValue_0_editingFinished();
    void on_EditValue_1_editingFinished();
    void on_EditValue_2_editingFinished();
    void on_EditValue_3_editingFinished();
    void on_EditValue_4_editingFinished();
    void on_EditValue_5_editingFinished();
    void on_EditValue_6_editingFinished();
    void on_EditValue_7_editingFinished();
    void on_EditValue_8_editingFinished();
    void on_EditValue_9_editingFinished();
    void on_EditValue_10_editingFinished();
    void on_EditValue_11_editingFinished();
    void on_EditValue_12_editingFinished();
    void on_EditValue_13_editingFinished();
    void on_EditValue_14_editingFinished();
    void on_EditValue_15_editingFinished();

    void onNoFocus(void);

signals:
    void ChangeValue(unsigned short adr, unsigned char wert);

private:
    bool ConvHex(QString str, unsigned char *value);
    void CheckEdit(QLineEdit *edit, unsigned short adresse);

    Ui::WidgetMemoryZeile *ui;
    unsigned short adresse;
};

#endif // WIDGET_MEMORY_ZEILE_H
