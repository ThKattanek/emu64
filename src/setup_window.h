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
// Letzte Änderung am 19.03.2022                //
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
#include "video_crt_class.h"
#include "c64_class.h"
#include "new_romset_window.h"
#include "user_palette_window.h"

namespace Ui {
    class SetupWindow;
}

class SetupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SetupWindow(QWidget *parent = 0, const char *member = 0, VideoCrtClass *video_crt_output = 0, QSettings *ini = 0, QString *romsetPath = 0, QString *dataPath = 0);
    ~SetupWindow();
    void RetranslateUi();
    void LoadINI(C64Class *c64);
    void SaveINI();
    void ReSetup(void);
    int GetScreenshotFormat();
	void DisableVideoCRT();			// Die Einstellmöglichkeit im Setup Window wird verhindert (--video-filter-off)
	void DisableTextureDouble();	// Die Einstellmöglichkeit im Setup Window wird verhindert (--texture-double-off)

protected:
	void showEvent(QShowEvent *event);

signals:
    void ChangeGrafikModi(bool fullscreen,bool palmode, bool doublemode, bool filter);
    void ResetSreenshotCounter(void);

private slots:
    void on_C64Farbmodus_currentIndexChanged(int index);
    void on_WPal_toggled(bool checked);
    void on_WDouble_toggled(bool checked);
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
    void on_VicGreyDotsEnable_toggled(bool checked);
    void on_default_pal_c64_clicked();
    void on_default_50hz_clicked();
    void on_cycles_per_second_valueChanged(int arg1);
    void on_Vsync_clicked(bool checked);
	void on_DisplayList_currentIndexChanged(int index);
	void on_VideoModes_currentIndexChanged(int index);
	void on_SettingUserPalette_clicked();
	void on_EnableUserPalette_clicked(bool checked);
	void OnChangeUserColor(int color_number, QColor color);


private:
    void UpdateToolTips();
    void FillRomSetCombo();
    QStringList GetAllRomsetNames(const QString *romset_dir);
    bool RemoveDir(const QString & dirName);

	int video_display_mode_index[MAX_VIDEO_DISPLAYS];
	bool is_filled_display_mode_list;

    Ui::SetupWindow *ui;
    QString *romsetPath;
    QString *dataPath;
    VideoCrtClass *video_crt_output;
    QSettings *ini;
    C64Class *c64;
};

#endif // SETUP_WINDOW_H
