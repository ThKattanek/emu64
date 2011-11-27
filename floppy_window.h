//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: floppy_window.h                       //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 12.10.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef FLOPPY_WINDOW_H
#define FLOPPY_WINDOW_H

#define FloppyAnzahl 4

#include <QtGui>
#include <QtCore>
#include <QSettings>
#include "d64_class.h"
#include "widget_d64_file.h"

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
    void OnSelectFile(QString filename);
    void OnChangeFloppyNummer(int floppynr);
    void OnRemoveImage(int floppynr);
    void on_FloppySelect_currentIndexChanged(int index);
    void on_D64Table_cellDoubleClicked(int row, int column);

private:
    Ui::FloppyWindow *ui;
    QFont *c64_font;
    QSettings *ini;
    bool isOneShowed;
    QString AktDir[FloppyAnzahl];
    QString AktFile[FloppyAnzahl];
    QString AktFileName[FloppyAnzahl];
    D64Class d64[FloppyAnzahl];
    void RefreshD64Table(void);
};

#endif // FLOPPY_WINDOW_H
