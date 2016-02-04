//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: main_window.h                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTextStream>
#include <QTranslator>
#include <QSettings>
#include <QActionGroup>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QBitmap>
#include <QRect>
#include <QThread>
#include <stdlib.h>

/// Windows ///
#include "custom_splashscreen.h"
#include "widget_floppy_status.h"
#include "info_window.h"
#include "tv_setup_window.h"
#include "floppy_window.h"
#include "c64_keyboard_window.h"
#include "crt_window.h"
#include "debugger_window.h"
#include "setup_window.h"
#include "c64_speed_window.h"

#include "c64_class.h"

#define loging 'if(log!=0) *log'
#define ErrorMsg(title, msg) QMessageBox::critical(this,title,msg,0,0);

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0,customSplashScreen* _splash = NULL,QTextStream *_log = NULL);
    ~MainWindow();
    Ui::MainWindow *ui;

    customSplashScreen *splash;

    QTextStream *log;
    void LogText(const char*);

    /// C64 Klasse ///
    C64Class *c64;
    VideoPalClass *videopal;

public slots:
    void OnMessage(QStringList msg);
    void OnInit();

private slots:
    void on_menu_main_info_triggered();
    void slotLanguageChanged(QAction* action);
    void on_actionBeenden_triggered();
    void on_actionSoftreset_triggered();
    void on_actionAutostart_triggered();
    void on_actionTV_Video_Einstellungen_triggered();
    void on_actionFloppy_1541_II_triggered();
    void on_actionC64_Tastaturbelegung_ndern_triggered();
    void on_actionVirtuelle_C64_Tastatur_triggered();
    void on_actionSteckmodule_triggered();
    void on_actionDebugger_Disassembler_triggered();
    void on_actionHardreset_triggered();
    void on_actionC64_Programme_direkt_laden_triggered();
    void on_actionEmu64_Einstellungen_triggered();
    void onChangeGrafikModi(bool fullscreen,bool palmode, bool doublemode, bool bit32mode, bool filter);
    void OnChangeFloppyImage(int floppynr);
    void OnResetScreenshotCounter(void);
    void on_actionScreenshot_triggered();
    void on_actionC64_Geschwindigkeit_triggered();
    void on_actionREU_einstecken_triggered();
    void on_actionREU_entfernen_triggered();
    void on_actionREU_laden_triggered();
    void on_actionREU_speichern_triggered();
    void on_actionREU_loeschen_triggered();
    void on_actionGEO_einstecken_triggered();
    void on_actionGEO_entfernen_triggered();
    void on_actionGEO_laden_triggered();
    void on_actionGEO_speichern_triggered();
    void on_actionGEO_loeschen_triggered();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    /// Funktionen ///
    void changeEvent(QEvent *event);  
    void CreateLanguageMenu(QString defaultLocale);
    void RetranslateUi();
    bool getSaveFileName(QWidget *parent, QString caption, QString filter, QString *fileName, QString *fileExt);
    void ExecuteCommandLine();

    /// Varialen ///
    QTranslator appTranslator;   // Application Translator
    QTranslator qtTranslator;    // Qt library Translator
    QString appPath;             // Pfad der emu64 Datei
    QString langPath;            // Pfad für alle Sprachfiles
    QString tmpPath;             // Pfad für alle Temporären Dateien die Emu64 erzeugt (Wird beim Start und Ende geleert)
    QString screenshotPath;       // Pfad für alle Emu64 Screenshots (nicht änderbar)
    QSettings *ini;

    bool ScreenshotsEnable;       // Konnte kein Screenshot Verzeichnis gefunden werden und auch nicht erstellt werden so ist es false
    int ScreenshotNumber;         // Nummer des nächsten Screenshots

    InfoWindow *info_window;                    // Info Fenster Klasse (wird vom MainWindow als Modal gestartet)
    TVSetupWindow *tv_setup_window;             // TV Einstellungsfenster
    FloppyWindow *floppy_window;                // Floppy Editor / Auswahl
    C64KeyboardWindow *c64_keyboard_window;     // Virtuelle C64 Tastatur
    CrtWindow *crt_window;                      // CRT Auswahl Fenster
    DebuggerWindow *debugger_window;            // Debugger / Disassembler Fenster
    SetupWindow *setup_window;                  // Setup Fenster
    C64SpeedWindow *speed_window;               // C64 Speed Window

    QStringList commandLine;                    // Enthält den an Emu64 übergebenen String
};

#endif // MAINWINDOW_H
