//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: new_romset_window.h                   //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 17.12.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef NEW_ROMSET_WINDOW_H
#define NEW_ROMSET_WINDOW_H

#include <QDialog>
#include <QString>
#include <QDir>
#include <QFileDialog>

namespace Ui {
class NewRomSetWindow;
}

class NewRomSetWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewRomSetWindow(QWidget *parent = 0, QString *romsetPath = 0);
    ~NewRomSetWindow();

    QString GetRomSetName();
    QString GetKernalRomFilename();
    QString GetBasicRomFilename();
    QString GetCharRomFilename();
    QString GetDos1541RomFilename();

private slots:
    void on_sel_kernal_rom_clicked();
    void on_sel_basic_rom_clicked();
    void on_sel_char_rom_clicked();
    void on_sel_dos1541_rom_clicked();

    void on_RomSetName_textChanged(const QString &arg1);

private:
    Ui::NewRomSetWindow *ui;
    QString *romsetPath;
};

#endif // NEW_ROMSET_WINDOW_H
