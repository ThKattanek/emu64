//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: crt_window.h                          //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 28.08.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef CRT_WINDOW_H
#define CRT_WINDOW_H

#include <QDialog>
#include <QSettings>

#include "crt_class.h"
#include "c64_class.h"
#include "structs.h"



namespace Ui {
    class CrtWindow;
}

class CrtWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CrtWindow(QWidget *parent = 0, QSettings *ini = 0);
    ~CrtWindow();
    void LoadIni(void);
    void RetranslateUi();
    void showEvent(QShowEvent *event);
    void ChangeLED(int,bool);

    CRTClass *crt;
    C64Class *c64;

private slots:
    void on_ViewChipData_clicked();
    void on_NewEasyFlashCRT_clicked();
    void onSelectFile(QString filename);
    void on_InsertCRT_clicked();
    void onChipList_currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_RemoveCRT_clicked();
    void on_FreezButtonFC3_clicked();
    void on_EF_JUMPER0_toggled(bool checked);

private:
    Ui::CrtWindow *ui;
    QSettings *ini;
    bool isOneShowed;
    bool ChipDataExpand;
    bool win_exp;
    unsigned short insterted_hwtyp;
    CRT_INFO_STRUCT crt_info;
    bool CRTIsSelected;
    QString SelCRTFileName;
    QIcon *LedRedOn;
    QIcon *LedRedOff;
};

#endif // CRT_WINDOW_H
