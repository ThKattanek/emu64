//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: setup_window.h                        //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 25.11.2018                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef SETUP_WINDOW_H
#define SETUP_WINDOW_H

#include <QDialog>
#include <QSettings>
#include <QTableWidgetItem>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

#include "button_mod.h"
#include "videopal_class.h"
#include "c64_class.h"
#include "new_romset_window.h"

namespace Ui {
    class SetupWindow;
}

class SetupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SetupWindow(QWidget *parent = 0, const char *member = 0, VideoPalClass *videopal = 0, QSettings *ini = 0, QString *romsetPath = 0, QString *dataPath = 0);
    ~SetupWindow();
    void RetranslateUi();
    void LoadINI(C64Class *c64);
    void SaveINI();
    void ReSetup(void);

    int GetScreenshotFormat();

signals:
    void ChangeGrafikModi(bool fullscreen,bool palmode, bool doublemode, bool bit32mode, bool filter);
    void ResetSreenshotCounter(void);

private slots:
    void on_C64Farbmodus_currentIndexChanged(int index);
    void on_WPal_toggled(bool checked);
    void on_WDouble_toggled(bool checked);
    void on_W16Bit_toggled(bool checked);
    void on_W32Bit_toggled(bool checked);
    void on_WFilter_toggled(bool checked);
    void on_ResetSShotCounter_clicked();
    void on_VJoySlots_cellChanged(int row, int column);
    void on_VJoySlots_cellClicked(int row, int column);
    void on_WAspectRatio_clicked(bool checked);
    void on_FAspectRatio_clicked(bool checked);
    void on_MausPort_currentIndexChanged(int index);
    void onClickButton(int idx, int idy);
    void on_SIDVolume_valueChanged(int value);
    void on_AutoMouseHide_clicked(bool checked);
    void on_AutoMouseHideTime_valueChanged(int arg1);
    void on_FirstSidTyp_currentIndexChanged(int index);
    void on_SecondSidTyp_currentIndexChanged(int index);
    void on_SecondSidAddress_currentIndexChanged(int index);
    void on_SecondSidEnable_toggled(bool checked);
    void on_SidCycleExactEnable_toggled(bool checked);
    void on_SidFilterEnable_toggled(bool checked);
    void on_Sid6ChannelMode_toggled(bool checked);
    void on_SelectRomSet_currentIndexChanged(const QString &arg1);
    void on_DeleteRomSet_clicked();
    void on_NewRomSet_clicked();

    void on_VicSpriteViewEnable_toggled(bool checked);

    void on_VicBorderViewEnable_toggled(bool checked);

    void on_VicSprSprCollisionEnable_toggled(bool checked);

    void on_VicSprBgrCollisionEnable_toggled(bool checked);

private:
    void UpdateToolTips();
    void FillRomSetCombo();
    QStringList GetAllRomsetNames(const QString *romset_dir);
    bool RemoveDir(const QString & dirName);

    Ui::SetupWindow *ui;

    QString *romsetPath;
    QString *dataPath;
    VideoPalClass *videopal;
    QSettings *ini;
    C64Class *c64;
};

#endif // SETUP_WINDOW_H
