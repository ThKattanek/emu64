//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: show_c64_key_mapping_window.h         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.12.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef SHOW_C64_KEY_MAPPING_WINDOW_H
#define SHOW_C64_KEY_MAPPING_WINDOW_H

#include <QDialog>

#include "./c64_class.h"

namespace Ui {
class ShowC64KeyMappingWindow;
}

class ShowC64KeyMappingWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ShowC64KeyMappingWindow(QWidget *parent = 0, C64Class *c64 = 0);
    ~ShowC64KeyMappingWindow();

private:
    Ui::ShowC64KeyMappingWindow *ui;
};

#endif // SHOW_C64_KEY_MAPPING_WINDOW_H
