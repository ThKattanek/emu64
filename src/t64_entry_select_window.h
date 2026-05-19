//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: t64_entry_select_window.h             //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef T64_ENTRY_SELECT_WINDOW_H
#define T64_ENTRY_SELECT_WINDOW_H

#include <QDialog>
#include <QFontDatabase>

#include "./t64_class.h"
#include "ui_t64_entry_select_window.h"

namespace Ui {
class T64EntrySelectWindow;
}

class T64EntrySelectWindow : public QDialog
{
    Q_OBJECT

public:
    explicit T64EntrySelectWindow(QWidget *parent = nullptr, T64Class* t64 = nullptr);
    ~T64EntrySelectWindow();

    int GetSelectedEntry()
    {
        if(ui->t64_entry_list->currentRow() >= 0)
            return ui->t64_entry_list->currentRow();
        else
            return 0;
    }

private slots:
    void on_OK_clicked();

    void on_t64_entry_list_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_clicked();

private:
    Ui::T64EntrySelectWindow *ui;

    QFont *c64_font1;
    QFont *c64_font2;

    T64Class* t64;
};

#endif // T64_ENTRY_SELECT_WINDOW_H
