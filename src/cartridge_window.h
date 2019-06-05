//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: crt_window.h                          //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 05.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef CRT_WINDOW_H
#define CRT_WINDOW_H

#include <QDialog>
#include <QSettings>
#include <QModelIndex>
#include <QTimer>

#include "cartridge_class.h"
#include "c64_class.h"
#include "structs.h"

#include "./cartridge_new_easyflash_window.h"

#define LED_COUNT 2

namespace Ui {
    class CrtWindow;
}

class CrtWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CrtWindow(QWidget *parent = 0, QSettings *ini = 0, C64Class *c64 = 0);
    ~CrtWindow();
    void LoadIni(void);
    bool SetCrtImage(QString filename);
    void ConnectCrt();
    void DisconnectCrt();
    void RetranslateUi();
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *);
    void ChangeLED(int,bool);

private slots:
    void on_ViewChipData_clicked();
    void on_NewEasyFlashCRT_clicked();
    void on_InsertCRT_clicked();
    void on_RemoveCRT_clicked();
    void on_FreezButtonFC3_clicked();
    void on_EF_JUMPER0_toggled(bool checked);
    void on_FreezButtonAR_clicked();

    void onSelectFile(QString filename);
    void onChipList_currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void onTimer1();

private:
    Ui::CrtWindow *ui;
    CRTClass *crt;
    C64Class *c64;
    QSettings *ini;
    bool isOneShowed;
    bool ChipDataExpand;
    bool win_exp;
    unsigned short insterted_hwtyp;
    CRT_INFO_STRUCT crt_info;
    bool CRTIsSelected;
    QString SelCRTFileName;
    QTimer *timer1;
    bool LedStatus[LED_COUNT];
    bool LedStatusOld[LED_COUNT];
    QIcon *LedRedOn;
    QIcon *LedRedOff;
};

#endif // CRT_WINDOW_H
