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
// Letzte Änderung am 24.07.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef FLOPPY_WINDOW_H
#define FLOPPY_WINDOW_H

#define FloppyAnzahl 4

#include <QDialog>
#include <QSettings>
#include "d64_class.h"

namespace Ui {
    class FloppyWindow;
}

class FloppyWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FloppyWindow(QWidget *parent = 0, QSettings *_ini = 0);
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

    void OnSelectFile(QString filename);
    void OnChangeFloppyNummer(int floppynr);
    void OnRemoveImage(int floppynr);
    void OnCustomMenuRequested(QPoint pos);
    void OnD64KontexMenu(QAction *actions);
    void on_ViewSplatFiles_clicked();

private:
    Ui::FloppyWindow *ui;
    QFont *c64_font;
    QSettings *ini;
    bool isOneShowed;
    QString AktDir[FloppyAnzahl];
    QString AktFile[FloppyAnzahl];
    QString AktFileName[FloppyAnzahl];
    D64Class d64[FloppyAnzahl];
    void RefreshD64FileList(void);


    QIcon *green_led ;
    QIcon *yellow_led;
    QIcon *red_led;

};

#endif // FLOPPY_WINDOW_H
