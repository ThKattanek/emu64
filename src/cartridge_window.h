//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: cartridge_window.h                    //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef CARTRIDGE_WINDOW_H
#define CARTRIDGE_WINDOW_H

#include <QDialog>
#include <QSettings>
#include <QModelIndex>
#include <QTimer>

#include "./cartridge_class.h"
#include "./custom_save_file_dialog.h"
#include "./c64_class.h"
#include "./structs.h"

#include "./cartridge_new_easyflash_window.h"

#define LED_NUM 2

namespace Ui {
    class CartridgeWindow;
}

class CartridgeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CartridgeWindow(QWidget *parent = nullptr, QSettings *ini = nullptr, C64Class *c64 = nullptr, QString tmp_path = nullptr);
    ~CartridgeWindow();
    void LoadIni(void);
    bool SetCartridgeImage(QString filename);
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

    void on_ExportChip_clicked();

private:
    Ui::CartridgeWindow *ui;
    CartridgeClass *crt;
    C64Class *c64;
    QString tmp_path;
    QSettings *ini;
    bool isOneShowed;
    bool ChipDataExpand;
    bool win_exp;
    unsigned short insterted_hwtyp;
    CARTRIDGE_INFO_STRUCT crt_info;
    bool CRTIsSelected;
    QString SelCRTFileName;
    QTimer *timer1;
    bool LedStatus[LED_NUM];
    bool LedStatusOld[LED_NUM];
    QIcon *LedRedOn;
    QIcon *LedRedOff;
};

#endif // CARTRIDGE_WINDOW_H
