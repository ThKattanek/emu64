//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: widget_floppy_status.h                //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 12.10.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#ifndef WIDGET_FLOPPY_STATUS_H
#define WIDGET_FLOPPY_STATUS_H

#include <QWidget>

#include "floppy1541_class.h"
#include "d64_class.h"

namespace Ui {
    class WidgetFloppyStatus;
}

class WidgetFloppyStatus : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetFloppyStatus(QWidget *parent = 0, int floppy_nr = 0, Floppy1541 *_floppy = 0);
    ~WidgetFloppyStatus();
    void SetGeraeteID(unsigned char id);
    QString GetAktFilename(void);
    bool GetEnableFloppy(void);
    void SetAktFilename(QString filename, QString d64name);
    void SetEnableFloppy(bool status);
    void SetFloppyVolume(int mode);
    int GetFloppyVolume(void);

signals:
   void ChangeEnableFloppy(void);
   void LoadImage(int floppynr);
   void RemoveImage(int floppynr);

private slots:
    void onTimer();
    void on_PowerLED_clicked();
    void on_InstertImage_clicked();
    void on_EjectImage_clicked();
    void on_Volume_clicked();

private:

    Ui::WidgetFloppyStatus *ui;

    int         FloppyNr;
    Floppy1541* floppy;

    QTimer *timer;

    QIcon *iGreenLedOff;
    QIcon *iGreenLedOn;
    QIcon *iYellowLedOff;
    QIcon *iYellowLedOn;

    QIcon *iVol_mute;
    QIcon *iVol_low;
    QIcon *iVol_medium;
    QIcon *iVol_high;

    int  FloppySoundVolumeMode;

    int  GeraeteID;
    bool PowerLED;
    bool MotorLED;
    bool RWLED;

    bool old_MotorLED;
    bool old_RWLED;
    unsigned char old_Sektor;
    unsigned char old_Spur;

    QString AktFileName;
};

#endif // WIDGET_FLOPPY_STATUS_H
