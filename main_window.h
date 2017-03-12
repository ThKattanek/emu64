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
// Letzte Änderung am 12.03.2017                //
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
#include <QDir>
#include <stdlib.h>

/// Windows ///
#include "./custom_splashscreen.h"
#include "./widget_floppy_status.h"
#include "./info_window.h"
#include "./tv_setup_window.h"
#include "./floppy_window.h"
#include "./c64_keyboard_window.h"
#include "./crt_window.h"
#include "./debugger_window.h"
#include "./setup_window.h"
#include "./c64_speed_window.h"
#include "./tape_window.h"
#include "./show_c64_key_mapping_window.h"

#include "./command_line_class.h"
#include "./emu64_commands.h"
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
    explicit MainWindow(QWidget *parent = 0, customSplashScreen* splash = NULL, QTextStream *log = NULL);
    ~MainWindow();
    Ui::MainWindow *ui;

    void paintEvent(QPaintEvent *event);

    customSplashScreen *splash;

    QTextStream *log;
    void LogText(const char*);

    /// C64 Klasse ///
    C64Class *c64;
    VideoPalClass *videopal;

    /// Close Event von der C64 Klasse
    void CloseC64Screeen(void);

    /// LimitCyclesEvent von der C64 Klasse
    void LimitCyclesEvent(void);
    bool IsLimitCyclesEvent;

    /// DebugCartEvent von der C64 Klasse
    void DebugCartEvent(unsigned char value);
    bool IsDebugCartEvent;
    unsigned char DebugCartValue;

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
    void on_actionVollbild_triggered();
    void on_actionBandlaufwerk_1530_triggered();

    void OnChangeGrafikModi(bool fullscreen,bool palmode, bool doublemode, bool bit32mode, bool filter);
    void OnChangeFloppyImage(int floppynr);
    void OnResetScreenshotCounter(void);
    void OnSetupFished(int result);

    void on_actionC64_Tastenbelegung_Show_triggered();

private:
    /// Funktionen ///
    void changeEvent(QEvent *event);  
    void CreateLanguageMenu(QString defaultLocale);
    void RetranslateUi();
    void SetC64ScreenTitle(void);
    bool getSaveFileName(QWidget *parent, QString caption, QString filter, QString *fileName, QString *fileExt);
    void ExecuteCommandLine(vector<char*> &arg);
    void SplashMessage(const QString &message, const QColor &color);

    /// Varialen ///
    QTranslator appTranslator;   // Application Translator
    QTranslator qtTranslator;    // Qt library Translator

    QString configPath;          // Pfad für emu64.ini und emu64.log
    QString dataPath;            // Pfad für alle Emu64 Daten

    //QString appPath;             // Pfad der emu64 Datei
    QString langPath;            // Pfad für alle Sprachfiles
    QString tmpPath;             // Pfad für alle Temporären Dateien die Emu64 erzeugt (Wird beim Start und Ende geleert)
    QString screenshotPath;      // Pfad für alle Emu64 Screenshots (nicht änderbar)
    QString romsetPath;          // Pfad für Benutzerdefinierte ROM-Sets

    QString lastAutoloadPath;    // Pfad der letzten Autoloadfunktion

    QSettings *ini;

    bool ScreenshotsEnable;       // Konnte kein Screenshot Verzeichnis gefunden werden und auch nicht erstellt werden so ist es false
    int ScreenshotNumber;         // Nummer des nächsten Screenshots

    bool RomSetEnable;            // False wenn kein RomSet-Verzeichnis existiert und keins erstellt werden konnte

    InfoWindow *info_window;                        // Info Fenster Klasse (wird vom MainWindow als Modal gestartet)
    TVSetupWindow *tv_setup_window;                 // TV Einstellungsfenster
    FloppyWindow *floppy_window;                    // Floppy Editor / Auswahl
    TapeWindow *tape_window;                        // Datasette
    C64KeyboardWindow *c64_keyboard_window;         // Virtuelle C64 Tastatur
    CrtWindow *crt_window;                          // CRT Auswahl Fenster
    DebuggerWindow *debugger_window;                // Debugger / Disassembler Fenster
    SetupWindow *setup_window;                      // Setup Fenster
    C64SpeedWindow *speed_window;                   // C64 Speed Window
    ShowC64KeyMappingWindow *show_c64keymap_window; //C64 Key Map Window

    //QStringList commandLine;                        // Enthält den an Emu64 übergebenen String

    bool isFirstPaintEvent;
};

#endif // MAINWINDOW_H
