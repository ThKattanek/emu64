//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: main_window.cpp                       //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 01.08.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include "main_window.h"
#include "ui_main_window.h"
#include "version.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /// Haputfenster UI setzen ///
    ui->setupUi(this);

#ifdef _WIN32
    setWindowTitle("Emu64 Version " + QString(str_emu64_version) + " --- [Win32]");
#endif

#ifdef __linux__
    setWindowTitle("Emu64 Version " + QString(str_emu64_version) + " --- [Linux]");
#endif

    setWindowIcon(QIcon(":/grafik/emu64.ico"));

    /// Klassen installieren ///
    videopal = new VideoPalClass();
    videopal->SetDisplayMode(color_bits);
    videopal->EnablePALOutput(false);
    videopal->EnableVideoDoubleSize(false);

    /// INI Dateiverwaltung erstellen ///
    ini = new QSettings("emu64.ini",QSettings::IniFormat,this);

    /// Window Klassen erstellen ///
    /// Unter MAC sollte ohne übergabe des this Zeigers die Klasseb erstellt werden

    info_window = new InfoWindow(this);
    tv_setup_window = new TVSetupWindow(this,videopal,ini);
    floppy_window = new FloppyWindow(this);
    c64_keyboard_window = new C64KeyboardWindow(this,ini);
    crt_window = new CrtWindow(this,ini);
    debugger_window = new DebuggerWindow(this,ini);

    /// Translator installieren ///
    langPath = "languages";
    qApp->installTranslator(&qtTranslator);
    qApp->installTranslator(&appTranslator);
    QString SystemLocale = QLocale::system().name();       // "de_DE"
    SystemLocale.truncate(SystemLocale.lastIndexOf('_'));  // "de"

    ini->beginGroup("MainWindow");
    CreateLanguageMenu(ini->value("lang",SystemLocale).toString());
    ini->endGroup();

    /// C64 Klasse Installieren ... Das HERZ ///
    int ret_error;
    c64 = new C64Class(&ret_error,0);
    if(ret_error != 0) ErrorMsg(tr("Emu64 Fehler ..."),tr("Fehler beim installieren der C64 Klasse"))

    c64->pal = videopal;
    videopal->SetPixelFormat(c64->C64ScreenBack->format);
    videopal->SetC64Palette(5);

    /// Debugger Window mit C64 verbinden ///
    debugger_window->SetC64Pointer(c64);

    /// CRT Klasse mit CRT Window verbinden ///
    crt_window->crt = c64->crt;
    crt_window->c64 = c64;

    /// C64 Systemroms laden ///
    if(!c64->LoadC64Roms((char*)"roms/kernal.rom",(char*)"roms/basic.rom",(char*)"roms/char.rom"))
    {
        ErrorMsg(tr("Emu64 Fehler ..."),tr("Fehler beim laden der C64 ROMs."))
    }

    /// C64 Keyboard Matrix mit dem Virtual Keyboard verbinden ///
    c64_keyboard_window->KeyMatrixToPA = c64->KeyboardMatrixToPAExt;
    c64_keyboard_window->KeyMatrixToPB = c64->KeyboardMatrixToPBExt;


    /// Tabelle für Floppy's Ertsellen ///
    ui->FloppyTabel->setRowCount(FloppyAnzahl);
    ui->FloppyTabel->setColumnCount(1);

    for(int i=0; i<FloppyAnzahl; i++)
    {
        ui->FloppyTabel->setRowHeight(i,24);
        WidgetFloppyStatus *w = new WidgetFloppyStatus(this,i,c64->floppy[i]);
        w->SetGeraeteID(i+8);
        ui->FloppyTabel->setCellWidget(i,0,w);
        ui->FloppyTabel->setColumnWidth(0,w->width());
        connect(w,SIGNAL(ChangeEnableFloppy()),debugger_window,SLOT(onChangeFloppyStatus()));
    }


    ////////// Load from INI ///////////
    if(ini != NULL)
    {
        ini->beginGroup("MainWindow");
        if(ini->contains("Geometry")) restoreGeometry(ini->value("Geometry").toByteArray());
        if(ini->contains("State")) restoreState(ini->value("State").toByteArray());
        ini->endGroup();

        char group_name[32];

        for(int i=0; i<FloppyAnzahl; i++)
        {
            sprintf(group_name,"Floppy1541_%2.2X",i+8);
            ini->beginGroup(group_name);
            WidgetFloppyStatus *w = (WidgetFloppyStatus*)ui->FloppyTabel->cellWidget(i,0);
            w->SetEnableFloppy(ini->value("Enabled",false).toBool());
            w->SetAktFilename(ini->value("AktFilename","").toString());
            w->SetFloppyVolume(ini->value("VolumeMode",2).toInt());
            ini->endGroup();
        }
    }
    this->update();
    /////////////////////////////////////

    /// CRT Ini erst jetzt laden ///
    crt_window->LoadIni();

    c64->HardReset();
}

MainWindow::~MainWindow()
{
    showNormal();
    //////////// Save to INI ////////////
    if(ini != NULL)
    {
        ini->beginGroup("MainWindow");
        ini->setValue("Geometry",saveGeometry());
        ini->setValue("State",saveState());
        ini->endGroup();

        char group_name[32];
        bool value;

        for(int i=0; i<FloppyAnzahl; i++)
        {
            sprintf(group_name,"Floppy1541_%2.2X",i+8);
            ini->beginGroup(group_name);
            WidgetFloppyStatus *w = (WidgetFloppyStatus*)ui->FloppyTabel->cellWidget(i,0);
            value = w->GetEnableFloppy();
            ini->setValue("Enabled",value);
            ini->setValue("AktFilename",w->GetAktFilename());
            ini->setValue("VolumeMode",w->GetFloppyVolume());
            ini->endGroup();
        }
    }
    /////////////////////////////////////


    /// WindowKlassen schließen ///

    delete c64;
    delete videopal;
    delete info_window;
    delete tv_setup_window;
    delete floppy_window;
    delete c64_keyboard_window;
    delete crt_window;
    delete debugger_window;
    delete ui;

    delete ini;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LocaleChange)
    {
      QString locale = QLocale::system().name();
      locale.truncate(locale.lastIndexOf('_'));

      qtTranslator.load("qt_" + locale, langPath);
      appTranslator.load("emu64_" + locale, langPath);
      RetranslateUi();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::CreateLanguageMenu(QString defaultLocale)
{
   QActionGroup* langGroup = new QActionGroup(ui->menuSprache);
   langGroup->setExclusive(true);

   connect(langGroup, SIGNAL(triggered(QAction *)),this, SLOT(slotLanguageChanged(QAction *)));

    QDir dir(langPath);
    QStringList fileNames;

    fileNames = dir.entryList(QStringList("emu64_*.qm"));

   for (int i = 0; i < fileNames.size(); ++i)
   {
      // get locale extracted by filename
      QString locale;
      locale = fileNames[i];                      // "emu64_de.qm"
      locale.truncate(locale.lastIndexOf('.'));   // "emu64_de"
      locale.remove(0, locale.indexOf('_') + 1);  // "de"

      QString lang = QLocale::languageToString(QLocale(locale).language());

      QAction *action = new QAction(lang, this);
      action->setCheckable(true);
      action->setData(locale);

      QString iconfile = fileNames[i];
      iconfile.truncate(iconfile.lastIndexOf('.'));
      iconfile = dir.filePath(iconfile + ".png");
      action->setIcon(QIcon(iconfile));
      action->setIconVisibleInMenu(true);
      action->setStatusTip(tr("Sprache: ") + lang);

      ui->menuSprache->addAction(action);
      langGroup->addAction(action);

      // set default translators and language checked
      if (defaultLocale == locale)
      {
         appTranslator.load("emu64_" + action->data().toString(), langPath);
         qtTranslator.load("qt_" + action->data().toString(), langPath);
         action->setChecked(true);
         ui->retranslateUi(this);
         RetranslateUi();
      }
    }
}

void MainWindow::slotLanguageChanged(QAction* action)
{
   appTranslator.load("emu64_" + action->data().toString(), langPath);
   qtTranslator.load("qt_" + action->data().toString(), langPath);
   ini->beginGroup("MainWindow");
   ini->setValue("lang",action->data().toString());
   ini->endGroup();
   RetranslateUi();
}

void MainWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    info_window->RetranslateUi();
    tv_setup_window->RetranslateUi();
    c64_keyboard_window->RetranslateUi();
    floppy_window->RetranslateUi();
    crt_window->RetranslateUi();
    debugger_window->RetranslateUi();
}

void MainWindow::on_menu_main_info_triggered()
{
    info_window->show();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    /*
    for(int i=0;i<64;i++)
    {
        if(C64KeyTable[i].nativeVirtualKey == event->nativeVirtualKey())
        {
            c64->KeyEvent(C64KeyTable[i].MatrixCode,KEY_DOWN);
        }
    }
    */
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    /*
    for(int i=0;i<64;i++)
    {
        if(C64KeyTable[i].nativeVirtualKey == event->nativeVirtualKey())
        {
            c64->KeyEvent(C64KeyTable[i].MatrixCode,KEY_UP);
        }
    }
    */
}

void MainWindow::on_actionBeenden_triggered()
{
    close();
}

void MainWindow::on_actionSoftreset_triggered()
{
    c64->SoftReset();
}

void MainWindow::on_actionHardreset_triggered()
{
    c64->HardReset();
}

void MainWindow::on_actionAutostart_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("C64 Dateien öffnen "),"",tr("C64 Programm Dateien") + "(*.prg *.p00 *.t64 *.d64 *.g64 *.frz);;" + tr("Alle Dateien") + "(*.*)");
    if(filename != "")
    {
        c64->LoadAutoRun(0,filename.toAscii().data());
    }
}

void MainWindow::on_actionC64_Programme_direkt_laden_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("C64 Dateien öffnen "),"",tr("C64 Programm Dateien") + "(*.prg *.p00 *.t64 *.frz);;" + tr("Alle Dateien") + "(*.*)");
    if(filename != "")
    {
        c64->LoadPRG(filename.toAscii().data(),0);
    }
}


void MainWindow::on_actionTV_Video_Einstellungen_triggered()
{
    tv_setup_window->show();
}

void MainWindow::on_actionFloppy_1541_II_triggered()
{
    floppy_window->show();
}

void MainWindow::on_actionVirtuelle_C64_Tastatur_triggered()
{
    c64_keyboard_window->Recording = false;
    c64_keyboard_window->show();
    c64_keyboard_window->update();
}

void MainWindow::on_actionC64_Tastaturbelegung_ndern_triggered()
{
    c64_keyboard_window->Recording = true;
    c64_keyboard_window->show();
    c64_keyboard_window->update();
}

void MainWindow::on_actionSteckmodule_triggered()
{
    if(crt_window->isHidden()) crt_window->show();
    else crt_window->hide();
}

void MainWindow::on_actionDebugger_Disassembler_triggered()
{
    if(debugger_window->isHidden()) debugger_window->show();
    else debugger_window->hide();
}
