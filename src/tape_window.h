//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tape_window.h                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 31.08.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef TAPE_WINDOW_H
#define TAPE_WINDOW_H

#include <QDialog>
#include <QSettings>
#include <QTimer>
#include <QShowEvent>

#include "c64_class.h"
#include "tape_new_window.h"

namespace Ui {
class TapeWindow;
}

class TapeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TapeWindow(QWidget *parent = nullptr, QSettings *_ini = nullptr, C64Class *c64 = nullptr, QString tmp_path = nullptr);
    ~TapeWindow();
    void RetranslateUi();
    void LoadIni();
    void showEvent(QShowEvent*);
    void hideEvent(QHideEvent*);

private slots:
    void OnSelectFile(QString filename);
    void OnRefreshGUI();

    void on_Rec_clicked();
    void on_Play_clicked();
    void on_Rew_clicked();
    void on_FFw_clicked();
    void on_Stop_clicked();

    void on_Volume_clicked();

private:
    void UpdateStateTapeKeys(unsigned char key_pressed);
    void SetTapeVolume(int mode);
    void AddFileExtension(QString &filename, const QString extension);

    Ui::TapeWindow *ui;

    QSettings *ini;
    C64Class *c64;

    QString tmp_path;

    QIcon *GreenLEDOff;
    QIcon *GreenLEDOn;

    QIcon *RedLEDOff;
    QIcon *RedLEDOn;

    int TapeSoundVolumeMode;

    QIcon *VolMute;
    QIcon *VolLow;
    QIcon *VolMedium;
    QIcon *VolHigh;

    bool isOneShowed;

    QTimer *refresh_timer;
};

#endif // TAPE_WINDOW_H
