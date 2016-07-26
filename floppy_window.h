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
// Letzte Änderung am 25.07.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef FLOPPY_WINDOW_H
#define FLOPPY_WINDOW_H

#define FloppyAnzahl 4

#include <QDialog>
#include <QMenu>
#include <QSettings>
#include "d64_class.h"
#include "c64_class.h"

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
    void showEvent(QShowEvent *event);
    QString GetAktFilename(int floppynr);
    QString GetAktD64Name(int floppynr);
    QStringList FileTypes;

signals:
    void ChangeFloppyImage(int floppynr);

private slots:
    void on_FloppySelect_currentIndexChanged(int index);
    void on_ViewSplatFiles_clicked();

    void OnSelectFile(QString filename);
    void OnChangeFloppyNummer(int floppynr);
    void OnRemoveImage(int floppynr);
    void OnCustomMenuRequested(QPoint pos);
    void OnD64FileStart0(bool);
    void OnD64FileStart1(bool);
    void OnD64FileStart2(bool);
    void OnD64FileStart3(bool);
    void OnPRGExport(bool);
    void OnPRGNameMMCKompatibel(bool);
    void on_D64FileTable_cellDoubleClicked(int row, int column);

private:
    void RefreshD64FileList(void);
    bool getSaveFileName(QWidget *parent, QString caption, QString filter, QString *fileName, QString *fileExt);

    Ui::FloppyWindow *ui;
    QFont *c64_font;
    QSettings *ini;
    C64Class *c64;
    bool isOneShowed;
    QString AktDir[FloppyAnzahl];
    QString AktFile[FloppyAnzahl];
    QString AktFileName[FloppyAnzahl];
    D64Class d64[FloppyAnzahl];

    QIcon *green_led ;
    QIcon *yellow_led;
    QIcon *red_led;

    QString TmpPath;

    bool CompatibleMMCFileName;
};

#endif // FLOPPY_WINDOW_H
