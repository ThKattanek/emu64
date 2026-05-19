//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: t64_entry_select_window.cpp           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./t64_entry_select_window.h"
#include "ui_t64_entry_select_window.h"

T64EntrySelectWindow::T64EntrySelectWindow(QWidget *parent, T64Class* t64)
    : QDialog(parent)
    , ui(new Ui::T64EntrySelectWindow)
{
    ui->setupUi(this);

    QFontDatabase::addApplicationFont(":/fonts/C64_Pro-STYLE.ttf");
    c64_font1 = new QFont("C64 Pro");
    c64_font1->setPixelSize(8);
    c64_font1->setStyleStrategy(QFont::PreferAntialias);
    c64_font1->setBold(false);
    c64_font1->setKerning(true);

    c64_font2 = new QFont("C64 Pro");
    c64_font2->setPixelSize(16);
    c64_font2->setStyleStrategy(QFont::PreferAntialias);
    c64_font2->setBold(false);
    c64_font2->setKerning(true);

    ui->t64_entry_list->setFont(*c64_font2);
    ui->t64_entry_list->setStyleSheet("color: rgb(100, 100, 255);");

    if(t64 != nullptr)
    {
        this->t64 = t64;
        for(uint16_t i=0; i<t64->GetTotalEntries(); i++)
        {
            QString entry_name = QString::fromStdString(t64->GetFileName(i)) + "\t $" + QString("%1").arg(t64->GetFileStartAddress(i), 4, 16, QChar('0')).toUpper();
            ui->t64_entry_list->addItem(entry_name);
        }
    }
    else
    {
        this->close();
    }
}

T64EntrySelectWindow::~T64EntrySelectWindow()
{
    delete ui;
    delete c64_font1;
    delete c64_font2;
}
void T64EntrySelectWindow::on_OK_clicked()
{
    accept();
}


void T64EntrySelectWindow::on_t64_entry_list_itemDoubleClicked(QListWidgetItem*)
{
    accept();
}


void T64EntrySelectWindow::on_pushButton_clicked()
{
    reject();
}

