//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy_window.h                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 11.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef FLOPPY_WINDOW_H
#define FLOPPY_WINDOW_H

#include <QDialog>
#include <QMenu>
#include <QSettings>
#include <QFontDatabase>
#include <QTimer>

#include "./d64_class.h"
#include "./c64_class.h"
#include "./floppy_new_d64_window.h"
#include "./custom_save_file_dialog.h"

namespace Ui {
    class FloppyWindow;
}

class FloppyWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FloppyWindow(QWidget *parent = 0, QSettings *_ini = 0, C64Class *c64 = 0, QString tmp_path = "");
    ~FloppyWindow();
    void RetranslateUi();
    void LoadIni();
    bool SetDiskImage(uint8_t floppynr, QString filename);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent*);
    QString GetAktFilename(int floppynr);
    QString GetAktD64Name(int floppynr);
    QStringList FileTypes;

signals:
    void ChangeFloppyImage(int floppynr);

private slots:
    void OnSelectFile(QString filename);
    void OnChangeFloppyNummer(int floppynr);
    void OnRemoveImage(int floppynr);
    void OnCustomMenuRequested(QPoint pos);
    void OnD64FileStart0(bool);
    void OnD64FileStart1(bool);
    void OnD64FileStart2(bool);
    void OnD64FileStart3(bool);
    void OnD64FileStart4(bool);
    void OnPRGExport(bool);
    void OnPRGNameMMCKompatibel(bool);
    void OnWriteProtectedChanged(bool);
    void OnCheckWriteDiskImageDir();

    void on_FloppySelect_currentIndexChanged(int index);
    void on_ViewSplatFiles_clicked();
    void on_D64FileTable_cellDoubleClicked(int, int);
    void on_D64FileTableBigSize_clicked(bool checked);
    void on_CreateNewD64_clicked();

private:
    void RefreshD64FileList(void);
    void SetD64BigSize(bool enable);
    QString ConvC64Name(const char* name, bool invers = false);

    Ui::FloppyWindow *ui;
    QTimer *refresh_timer_1;    // Timer für Prüfung auf Schreibzugriff auf Image Directory


    QFont *c64_font;
    QFont *c64_font1;   // NormalSize (18)
    QFont *c64_font2;   // BigSize (36)
    QSettings *ini;
    C64Class *c64;
    bool isOneShowed;
    QString AktDir[MAX_FLOPPY_NUM];
    QString AktFile[MAX_FLOPPY_NUM];
    QString AktFileName[MAX_FLOPPY_NUM];
    D64Class d64[MAX_FLOPPY_NUM];

    QIcon *green_led ;
    QIcon *yellow_led;
    QIcon *red_led;

    QIcon *green_led_small;
    QIcon *yellow_led_small;
    QIcon *red_led_small;

    QIcon *green_led_big;
    QIcon *yellow_led_big;
    QIcon *red_led_big;

    QString tmp_path;

    bool CompatibleMMCFileName;
};

#endif // FLOPPY_WINDOW_H
