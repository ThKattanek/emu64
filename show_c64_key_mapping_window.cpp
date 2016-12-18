//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: show_c64_key_mapping_window.cpp       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.12.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "show_c64_key_mapping_window.h"
#include "ui_show_c64_key_mapping_window.h"

ShowC64KeyMappingWindow::ShowC64KeyMappingWindow(QWidget *parent, C64Class *c64) :
    QDialog(parent),
    ui(new Ui::ShowC64KeyMappingWindow)
{
    ui->setupUi(this);
}

ShowC64KeyMappingWindow::~ShowC64KeyMappingWindow()
{
    delete ui;
}
