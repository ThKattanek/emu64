//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: memory_window.h                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.06.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MEMORY_WINDOW_H
#define MEMORY_WINDOW_H

#include <QDialog>
#include <QTableWidgetItem>

#include "c64_class.h"
#include "widget_memory_zeile.h"

#define MAX_MEMORY_ROW 256

namespace Ui {
    class MemoryWindow;
}

class MemoryWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MemoryWindow(QWidget *parent = 0);
    ~MemoryWindow();
    void RetranslateUi();
    void SetC64Pointer(C64Class *_c64);
    void ChangeSource(int source);
    void UpdateMemoryList(void);

private slots:
    void onChangeValue(unsigned short adresse, unsigned char wert);
    void on_MemoryScroll_valueChanged(int value);
    void on_MemoryScroll_sliderPressed();
    void on_MemoryScroll_sliderReleased();
    void on_OnlyRam_clicked(bool checked);

    void on_jump_address_returnPressed();

signals:
    void NoFocus(void);

private:
    Ui::MemoryWindow *ui;
    C64Class *c64;
    int AktSource;
    int AktFloppyNr;
    int AktViewAdresse;
    bool NoFocusRun;

    /// FloppyMemoryMap Visuall //
    unsigned char MapReadSource[0x100];
    unsigned char MapWriteDestination[0x100];

    QStringList MemScrDest;
    QStringList FloppyMemScrDest;

    int16_t memory_rows;
    WidgetMemoryZeile *memory_row[MAX_MEMORY_ROW];
};

#endif // MEMORY_WINDOW_H
