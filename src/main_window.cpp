//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: main_window.cpp                       //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 04.07.2021                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QScreen>
#include <QStyle>

#include "./main_window.h"
#include "./ui_main_window.h"
#include "./c64_file_types.h"
#include "./utils.h"

static QMutex mutex_log_text;

MainWindow::MainWindow(QWidget *parent,CustomSplashScreen* splash,QTextStream *log) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    c64(nullptr)
{
    this->splash = splash;
    this->log = log;

    isFirstPaintEvent = false;

    this->setGeometry(0,0,0,0);

    ///
    setup_window = nullptr;
    video_crt_output = nullptr;
    info_window = nullptr;
    video_crt_setup_window = nullptr;
    floppy_window = nullptr;
    tape_window = nullptr;
    c64_keyboard_window = nullptr;
    cartridge_window = nullptr;
    debugger_window = nullptr;
    speed_window = nullptr;
    show_c64keymap_window = nullptr;
    video_capture_window = nullptr;
    sid_dump_window = nullptr;
    oscilloscope_window = nullptr;

    /// Haputfenster UI setzen ///
    ui->setupUi(this);

    // Center Window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::screens()[0]->availableGeometry()));

#ifdef _WIN32
    setWindowTitle("Emu64 Version " + QString(VERSION_STRING) + " --- [Windows " + QString(ARCHITECTURE_STRING) + "]");
#else
# ifdef __linux__
    setWindowTitle("Emu64 Version " + QString(VERSION_STRING) + " --- [Linux " + QString(ARCHITECTURE_STRING) + "]");
# else
#  ifdef __FreeBSD__
    setWindowTitle("Emu64 Version " + QString(VERSION_STRING) + " --- [FreeBSD " + QString(ARCHITECTURE_STRING) + "]");
#  else
    setWindowTitle("Emu64 Version " + QString(VERSION_STRING) + " --- [POSIX (unknown) " + QString(ARCHITECTURE_STRING) + "]");
#  endif
# endif
#endif
}

MainWindow::~MainWindow()
{
    //////////// Save to INI ////////////
    if(ini != nullptr && no_write_ini_exit == false)
    {
        ini->beginGroup("MainWindow");
        ini->setValue("Geometry",saveGeometry());
        ini->setValue("State",saveState());
        ini->setValue("ScreenshotCounter",c64->GetScreenshotNumber());
        ini->setValue("LastAutoloadDir",lastAutoloadPath);
        ini->endGroup();

        char group_name[32];
        bool value;

        for(int i=0; i<MAX_FLOPPY_NUM; i++)
        {
            sprintf(group_name,"Floppy1541_%2.2X",i+8);
            ini->beginGroup(group_name);
            WidgetFloppyStatus *w = (WidgetFloppyStatus*)ui->FloppyTabel->cellWidget(i,0);
            value = w->GetEnableFloppy();
            ini->setValue("Enabled",value);
            ini->setValue("VolumeMode",w->GetFloppyVolume());
            ini->endGroup();
        }

        int x,y,w,h;
        c64->GetWindowPos(&x, &y);
        c64->GetWindowSize(&w, &h);

        ini->beginGroup("C64Screen");
        ini->setValue("PosX",x);
        ini->setValue("PosY",y);
        ini->setValue("SizeW",w);
        ini->setValue("SizeH",h);
        ini->endGroup();

        C64_KEYS* c64_key_table = c64->GetC64KeyTable();
        int c64_key_table_size = c64->GetC64KeyTableSize();

        ini->beginGroup("C64KeyMapping");

        for(int i=0; i<c64_key_table_size; i++)
        {
            QString name = "C64_KEY_" + QVariant(i).toString();
            ini->setValue(name,(int)c64_key_table[i].SDLKeyCode);
        }
        ini->endGroup();

        ini->beginGroup("Sound");
        ini->setValue("SoundBufferSize",soundbuffer_size);
        ini->endGroup();
    }
    /////////////////////////////////////

    /// WindowKlassen schließen ///

    if(setup_window != nullptr) delete setup_window;
    if(c64 != nullptr) delete c64;

    if(video_crt_output != nullptr) delete video_crt_output;
    if(info_window != nullptr)delete info_window;
    if(video_crt_setup_window != nullptr)delete video_crt_setup_window;
    if(floppy_window != nullptr)delete floppy_window;
    if(tape_window != nullptr)delete tape_window;
    if(c64_keyboard_window != nullptr)delete c64_keyboard_window;
    if(cartridge_window != nullptr)delete cartridge_window;
    if(debugger_window != nullptr)delete debugger_window;
    if(speed_window != nullptr)delete speed_window;
    if(show_c64keymap_window != nullptr)delete show_c64keymap_window;
    if(video_capture_window != nullptr)delete video_capture_window;
    if(sid_dump_window != nullptr)delete sid_dump_window;
    if(oscilloscope_window != nullptr)delete oscilloscope_window;

    delete ui;
    delete ini;
    LogText(tr(">> Es wurden alle Klassen wieder entfernt\n").toUtf8());

    LogText(tr("\n>> Emu64 wurde sauber beendet...").toUtf8());
    delete log;

    // Alle temporären Dateien löschen ausser Verzeichnisse (legt Emu64 auch nicht an)
    if(emu64_tmp.exists())
    {
        QStringList files = emu64_tmp.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Files);
        for(int i=0; i<files.count(); i++)
        {
            QFile::remove(emu64_tmp.path() + "/" + files[i]);
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if(!isFirstPaintEvent)
    {
        isFirstPaintEvent = true;
        if(c64 != nullptr) c64->SetFocusToC64Window();
    }
}

int MainWindow::OnInit()
{
    // Alle Pfade setzen //

    configPath = QDir::homePath() + "/.config/emu64";

#ifdef _WIN32
    if(custom_dataPath == "")
        dataPath = QApplication::applicationDirPath();
    else
        dataPath = custom_dataPath;
#else
    if(custom_dataPath == "")
    {
        dataPath = DATA_PATH;
        dataPath += "/share/emu64";
    }
    else
        dataPath = custom_dataPath;
#endif
    LogText((QString(">> Data Path = ") + dataPath + QString("\n")).toUtf8());

    /// Emu64 Temporäres Verzeichnis ermitteln, anlegen, ...
    QDir tmp = QDir::temp();

    if(tmp.exists())
    {
        LogText((QString(">> System Temp Path: ") + tmp.path() + QString("\n")).toUtf8());

        emu64_tmp = QDir(tmp.path() + "/emu64");

        if(!emu64_tmp.exists())
        {
            LogText((QString(">> Emu64 Temp Verzeichnis existiert nicht und wird erstellt.") + emu64_tmp.path() + QString("\n")).toUtf8());
            if(!tmp.mkdir("emu64"))
            {
                LogText((QString(">> Emu64 Temp Verzeichnis konnte nicht ermittelt werden.") + emu64_tmp.path() + QString("\n")).toUtf8());
            }
        }

        if(!emu64_tmp.exists())
        {
            LogText((QString(">> Emu64 Temp Path konnte nicht ermittelt werden.") + emu64_tmp.path() + QString("\n")).toUtf8());
            emu64_tmp.path() = nullptr;
        }
        else
        {
            emu64_tmp.path() = emu64_tmp.path();
            LogText((QString(">> Emu64 Temp Path: ") + emu64_tmp.path() + QString("\n")).toUtf8());
        }
    }
    else {
        LogText((QString(">> System Temp Path konnte nicht ermittelt werden.")).toUtf8());
        tmp.path() = nullptr;
        LogText((QString(">> Emu64 Temp Path konnte nicht ermittelt werden.")).toUtf8());
        emu64_tmp.path() = nullptr;
    }

    screenshotPath = configPath + "/screenshots";
    LogText((QString(">> Screenshot Path = ") + screenshotPath + QString("\n")).toUtf8());

    if(splash != nullptr)
    {
        splash->SetMessageRect(QRect(0,303,400, 20), Qt::AlignCenter);

        QFont splashFont;
        splashFont.setFamily("Arial");
        splashFont.setBold(true);
        splashFont.setPixelSize(10);
        splashFont.setStretch(125);

        splash->setFont(splashFont);
    }

    /// Translator installieren ///
    langPath = dataPath+"/languages";
    qApp->installTranslator(&qtTranslator);
    qApp->installTranslator(&appTranslator);
    QString SystemLocale = QLocale::system().name();       // "de_DE"
    SystemLocale.truncate(SystemLocale.lastIndexOf('_'));  // "de"

    /// INI Dateiverwaltung erstellen ///
    SplashMessage(tr("INI Dateiverwaltung wird initialisiert."),Qt::darkBlue);

    ini = new QSettings(configPath+"/emu64.ini",QSettings::IniFormat,this);

    LogText(QString(">> INI System wurde erzeugt: " + configPath + "/emu64.ini\n").toUtf8());
    LogText(QString(">> INI System was created: " + configPath + "/emu64.ini\n").toUtf8());

    SplashMessage(tr("Sprachmenü wir erstellt."),Qt::darkBlue);
    ini->beginGroup("MainWindow");
    CreateLanguageMenu(ini->value("lang",SystemLocale).toString());

    lastAutoloadPath = ini->value("LastAutoloadDir",QDir::homePath()).toString();

    ini->endGroup();
    LogText(QString(tr(">> Sprachmenü wurde erstellt.") + SystemLocale + "\n").toUtf8());


    SplashMessage(tr("Translator wurde installiert."),Qt::darkBlue);
    LogText(QString(tr(">> Translator wurde intsalliert: Systemsprache = ") + SystemLocale + "\n").toUtf8());
    setWindowIcon(QIcon(":/grafik/emu64.ico"));

    /// RomSetPath erstellen ///
    romsetPath = configPath + "/romsets/";

    QDir* dir = new QDir(romsetPath);

    SplashMessage(tr("RomSet-Verzeichnis wird gesucht."),Qt::darkBlue);
    if(dir->exists())
    {
        LogText(tr(">> Ein RomSet Verzeichnis ist vorhanden\n").toUtf8());
        RomSetEnable = true;
    }
    else
    {
        SplashMessage(tr("RomSet-Verzeichnis wird erstellt."),Qt::darkBlue);
        LogText(tr("<< Ein RomSet-Verzeichnis ist nicht vorhanden\n").toUtf8());
        if(dir->mkdir(romsetPath))
        {
            LogText(tr(">> Ein neues RomSet-Verzeichnis wurde erstellt\n").toUtf8());
            RomSetEnable = true;
        }
        else
        {
            LogText(tr("<< Ein neues RomSet-Verzeichnis konnte nicht erstellt werden\n").toUtf8());
            RomSetEnable = false;
        }
    }

    delete dir;

    if(RomSetEnable)
    {
        LogText(">> romsetPath = ");
        LogText(romsetPath.toUtf8());
        LogText("\n");
    }

    dir = new QDir(screenshotPath);
    SplashMessage(tr("Screenshotverzeichnis wird gesucht."),Qt::darkBlue);
    if(dir->exists())
    {
        LogText(tr(">> Das Screenshot Verzeichnis ist vorhanden\n").toUtf8());
        ScreenshotsEnable = true;
    }
    else
    {
        SplashMessage(tr("Das Screenshotverzeichnis wird erstellt."),Qt::darkBlue);
        LogText(tr("<< Das Screenshot Verzeichnis ist nicht vorhanden\n").toUtf8());
        if(dir->mkdir(screenshotPath))
        {
            LogText(tr(">> Ein neues Screenshot Verzeichnis wurde erstellt\n").toUtf8());
            ScreenshotsEnable = true;
        }
        else
        {
            LogText(tr("<< Ein neues Screenshot Verzeichnis konnte nicht erstellt werden\n<< Keine Screenshots moeglich !!").toUtf8());
            ScreenshotsEnable = false;
        }
    }
    delete dir;

    /// Klassen installieren ///
    SplashMessage(tr("VideoPal Klasse wird initialisiert."),Qt::darkBlue);
    video_crt_output = new VideoCrtClass();
    LogText(tr(">> VideoPal Klasse wurde installiert\n").toUtf8());

    /// C64 Klasse Installieren ... Das HERZ ///
    ini->beginGroup("Sound");
    soundbuffer_size = ini->value("SoundBufferSize",512).toInt();
    ini->endGroup();

    if(soundbuffer_size < 64)
        soundbuffer_size = 64;

    SplashMessage(tr("C64 Klasse wird initialisiert."),Qt::darkBlue);
    int ret_error;
    c64 = new C64Class(&ret_error ,soundbuffer_size ,video_crt_output,start_minimized, bind(&MainWindow::LogText,this,std::placeholders::_1),QString(dataPath).toLocal8Bit());
    if(ret_error != 0)
    {
        LogText(tr("<< Fehler beim initiallisieren der C64 Klasse.\n").toUtf8());
        this->close();
        return -1;
    }

    SetC64ScreenTitle();
    c64->EnableScreenshots(ScreenshotsEnable);
    c64->SetScreenshotDir(screenshotPath.toLocal8Bit());

    /// Window Klassen erstellen ///
    /// Unter MAC sollte ohne übergabe des this Zeigers die Klasseb erstellt werden

    SplashMessage(tr("InfoWindow wird erstellt."),Qt::darkBlue);
    info_window = new InfoWindow(this);
    LogText(tr(">> InfoWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("VideoCrtSetupWindow wird erstellt."),Qt::darkBlue);
    video_crt_setup_window = new VideoCrtSetupWindow(this,c64,video_crt_output,ini);
    LogText(tr(">> VideoCrtSetupWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("FloppyWindow wird erstellt."),Qt::darkBlue);
    floppy_window = new FloppyWindow(this,ini,c64,emu64_tmp.path());
    LogText(tr(">> FloppyWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("TapeWindow wird erstellt."),Qt::darkBlue);
    tape_window = new TapeWindow(this,ini,c64,emu64_tmp.path());
    LogText(tr(">> TapeWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("C64KeyboardWindow wird erstellt."),Qt::darkBlue);
    c64_keyboard_window = new C64KeyboardWindow(this,ini,c64);
    LogText(tr(">> C64KeyboardWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("CRTWindow wird erstellt."),Qt::darkBlue);
    cartridge_window = new CartridgeWindow(this,ini,c64,emu64_tmp.path());
    LogText(tr(">> CrtWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("DebuggerWindow wird erstellt."),Qt::darkBlue);
    debugger_window = new DebuggerWindow(this,ini);
    LogText(tr(">> DebuggerWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("SetupWindow wird erstellt."),Qt::darkBlue);
    setup_window = new SetupWindow(this,SLOT(OnChangeGrafikModi(bool,bool,bool,bool)),video_crt_output,ini,&romsetPath,&dataPath);
    connect(setup_window,SIGNAL(finished(int)),this,SLOT(OnSetupFished(int)));
    LogText(tr(">> SetupWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("C64SpeedWindow wird erstellt."),Qt::darkBlue);
    speed_window = new C64SpeedWindow(this,ini);
    LogText(tr(">> C64SpeedWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("ShowC64KeyMapWindow wird erstellt."),Qt::darkBlue);
    show_c64keymap_window = new ShowC64KeyMappingWindow(this,c64);
    LogText(tr(">> ShowC64KeyMapWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("VideoCaptureWindow wird erstellt."),Qt::darkBlue);
    video_capture_window = new VideoCaptureWindow(this, c64);
    LogText(tr(">> VideoCaptureWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("SidDumpWindow wird erstellt."),Qt::darkBlue);
    sid_dump_window = new SidDumpWindow(this, c64);
    LogText(tr(">> SidDumpWindow wurde erzeugt\n").toUtf8());

    SplashMessage(tr("OscilloscopeWindow wird erstellt."),Qt::darkBlue);
    oscilloscope_window = new OscilloscopeWindow(this, c64->GetAudioSampleRate());
    LogText(tr(">> OscilloscopeWindow wurde erzeugt\n").toUtf8());

    ini->beginGroup("MainWindow");
    SplashMessage(tr("Screenshotnummer wird geladen."),Qt::darkBlue);
    uint32_t screenshot_number;
    screenshot_number = (int)ini->value("ScreenshotCounter",0).toInt();
    c64->SetScreenshotNumber(screenshot_number);
    ini->endGroup();
    LogText(tr(">> Aktuelle Screenshotnummer wurde geladen\n").toUtf8());

    //SplashMessage(tr("SetupWindow wird mit INI abgeglichen."),Qt::darkBlue);
    //setup_window->ReSetup();
    //LogText(tr(">> SetupWindow ReSetup wurde durchgeführt\n").toUtf8());

    //SplashMessage(tr("SDL Window Titelleiste wird benannt."),Qt::darkBlue);
    //SDL_WM_SetCaption((const char*)tr("C64 Bildschirm").toUtf8(),0);

    /// Debugger Window mit C64 verbinden ///
    SplashMessage(tr("Debugger Window wird mit C64 Klasse verbunden."),Qt::darkBlue);
    debugger_window->SetC64Pointer(c64);
    LogText(tr(">> DebuggerWindow wurde mit C64 Klasse verbunden\n").toUtf8());

    /// C64 Speed Window mit C64 verbinden ///
    SplashMessage(tr("C64 Speed Window wird mit C64 Klasse verbunden."),Qt::darkBlue);
    speed_window->SetC64Pointer(c64);
    LogText(tr(">> C64SpeedWindow wurde mit C64 Klasse verbunden\n").toUtf8());

    /// CRT LED mit CRT_Window verbinden ///
    SplashMessage(tr("CRT LED mit CRT Window verbunden."),Qt::darkBlue);
    c64->crt->ChangeLED = bind(&CartridgeWindow::ChangeLED,cartridge_window,std::placeholders::_1,std::placeholders::_2);
    LogText(tr(">> CRT LED wurde mit CrtWindow verbunden\n").toUtf8());

    /// C64 Systemroms laden ///
    SplashMessage(tr("C64 Systemroms werden geladen."),Qt::darkBlue);
    if(!c64->LoadC64Roms(QString(dataPath+"/roms/kernal.rom").toLocal8Bit(),QString(dataPath+"/roms/basic.rom").toLocal8Bit(),QString(dataPath+"/roms/char.rom").toLocal8Bit()))
    {
        LogText((char*)"<< ERROR: Fehler beim laden der C64 Roms\n\t");
        LogText(QString(dataPath+"/roms/kernal.rom").toUtf8());LogText("\n\t");
        LogText(QString(dataPath+"/roms/basic.rom").toUtf8());LogText("\n\t");
        LogText(QString(dataPath+"/roms/char.rom").toUtf8());LogText("\n");
    }
    else LogText(tr(">> Alle C64 Roms geladen\n").toUtf8());

    /// C64 Keyboard Matrix mit dem Virtual Keyboard verbinden ///
    SplashMessage(tr("C64 Key-Matrix wird mit Virtual Keyboard verbunden."),Qt::darkBlue);
    c64_keyboard_window->key_matrix_to_port_a = c64->key_matrix_to_port_a_ext;
    c64_keyboard_window->key_matrix_to_port_b = c64->key_matrix_to_port_b_ext;
    LogText(tr(">> C64 Key-Matrix wurde mit Virtual Keyboard verbunden\n").toUtf8());


    /// Tabelle für Floppy's Ertsellen ///
    SplashMessage(tr("Tabelle für Floppy's wird erstellt."),Qt::darkBlue);
    ui->FloppyTabel->setRowCount(MAX_FLOPPY_NUM);
    ui->FloppyTabel->setColumnCount(1);
    LogText(tr(">> Tabelle für alle Floppy Laufwerke wurde erzeugt\n").toUtf8());

    for(int i=0; i<MAX_FLOPPY_NUM; i++)
    {
        SplashMessage(tr("Floppy: ") + QVariant(i).toString() + "wird in Tabelle eingefuegt",Qt::darkBlue);
		ui->FloppyTabel->setRowHeight(i,24);
        WidgetFloppyStatus *w = new WidgetFloppyStatus(this,i,c64->floppy[i]);
        w->SetGeraeteID(i+8);
        ui->FloppyTabel->setCellWidget(i,0,w);
        ui->FloppyTabel->setColumnWidth(0,w->width());
        connect(w,SIGNAL(ChangeEnableFloppy()),debugger_window,SLOT(onChangeFloppyStatus()));
        connect(w,SIGNAL(LoadImage(int)),floppy_window,SLOT(OnChangeFloppyNummer(int)));
        connect(w,SIGNAL(RemoveImage(int)),floppy_window,SLOT(OnRemoveImage(int)));
    }
    LogText(tr(">> Alle Floppy Lauferke in Tabelle eingetragen\n").toUtf8());

    /// Close Evend von C64 Klasse hierher leiten
    c64->CloseEventC64Screen = bind(&MainWindow::CloseC64Screeen,this);
    LogText(tr(">> SDL Window Close Event mit MainWindow verknuepft\n").toUtf8());

    /// LimitCyclesEvent von C64 Klasse hierher leiten
    IsLimitCyclesEvent = false;
    c64->LimitCyclesEvent = bind(&MainWindow::LimitCyclesEvent,this);
    LogText(tr(">> C64Class LimitCycelsEvent mit MainWindow verknuepft\n").toUtf8());

    /// DebugCartEvent von C64 Klasse hierher leiten
    IsDebugCartEvent = false;
    c64->DebugCartEvent = bind(&MainWindow::DebugCartEvent,this,std::placeholders::_1);
    LogText(tr(">> C64Class DebugCartEvent mit MainWindow verknuepft\n").toUtf8());

    connect(floppy_window,SIGNAL(ChangeFloppyImage(int)),this,SLOT(OnChangeFloppyImage(int)));
    LogText(tr(">> ChangeFloppyImage Event mit MainWindow verknuepft\n").toUtf8());

    /// Audio mit Oszilloskope verbinden
    c64->AudioOutProc = std::bind(&OscilloscopeWindow::AudioStreamEvent,oscilloscope_window,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4);

    ////////// Load from INI ///////////
    SplashMessage(tr("Einstellungen werden von INI geladen und gesetzt."),Qt::darkBlue);
    if(ini != nullptr)
    {
        floppy_window->LoadIni();
        LogText(tr(">> FloppyWindow LoadIni wurde ausgefuehrt\n").toUtf8());

        ini->beginGroup("MainWindow");
        if(ini->contains("Geometry")) restoreGeometry(ini->value("Geometry").toByteArray());
        if(ini->contains("State")) restoreState(ini->value("State").toByteArray());
        ini->endGroup();

        char group_name[32];

        for(int i=0; i<MAX_FLOPPY_NUM; i++)
        {
            sprintf(group_name,"Floppy1541_%2.2X",i+8);
            ini->beginGroup(group_name);
            WidgetFloppyStatus *w = (WidgetFloppyStatus*)ui->FloppyTabel->cellWidget(i,0);

            w->SetAktFilename(floppy_window->GetAktFilename(i),floppy_window->GetAktD64Name(i));

			QFileInfo file_info(floppy_window->GetAktFilename(i));

			int typ = NO_C64_FILE;
			if(file_info.completeSuffix().toUpper() == "D64")
				typ = D64;

			if(file_info.completeSuffix().toUpper() == "G64")
				typ = G64;

			FILE *file = qfopen(floppy_window->GetAktFilename(i), "r+b");
			c64->LoadDiskImage(i,file, typ);

            w->SetEnableFloppy(ini->value("Enabled",false).toBool());
            w->SetFloppyVolume(ini->value("VolumeMode",2).toInt());
            ini->endGroup();
        }

        /// SETUP Ini laden ///
        setup_window->LoadINI(c64);
        LogText(tr(">> SetupWindow LoadIni wurde ausgefuehrt\n").toUtf8());

        c64->SetScreenshotFormat(setup_window->GetScreenshotFormat());

        /// CRT Ini erst jetzt laden ///
        cartridge_window->LoadIni();
        LogText(tr(">> CrtWindow LoadIni wurde ausgefuehrt\n").toUtf8());

        /// TAPE Ini laden ///
        tape_window->LoadIni();
        LogText(tr(">> TapeWindow LoadIni wurde ausgefuehrt\n").toUtf8());

        /// C64 Key Mapping aus INI laden ///

        C64_KEYS* c64_key_table = c64->GetC64KeyTable();
        int c64_key_table_size = c64->GetC64KeyTableSize();

        ini->beginGroup("C64KeyMapping");

        for(int i=0; i<c64_key_table_size; i++)
        {
            QString name = "C64_KEY_" + QVariant(i).toString();
            unsigned long sdl_key_code = ini->value(name,0xFFFFFFFF).toUInt();

            if(sdl_key_code != 0xFFFFFFFF)
            {
                c64_key_table[i].SDLKeyCode = sdl_key_code;
            }
        }

        LogText(tr(">> C64 Tastatur Mapping aus INI geladen\n").toUtf8());

        ini->endGroup();
    }

    this->update();

    /// Screenshot Reset mit SetupWindow verbinden ///
    connect(setup_window,SIGNAL(ResetSreenshotCounter()),this,SLOT(OnResetScreenshotCounter()));


    ini->beginGroup("C64Screen");

    int x = ini->value("PosX",-1).toInt();
    int y = ini->value("PosY",-1).toInt();

    if((x>0) && (y>0))
    {
        c64->SetWindowPos(x,y);
    }

    int w = ini->value("SizeW",-1).toInt();
    int h = ini->value("SizeH",-1).toInt();

    if((w>0) && (h>0))
    {
        c64->SetWindowSize(w,h);
    }

    ini->endGroup();

    LogText(">> Emu64 wurde initialisiert\n\n\n");

    if(splash != nullptr) splash->close();
    LogText(tr(">> Splashsreen wurde geschlossen.\n").toUtf8());

    this->show();

    /////////////////////////////////////
    SplashMessage(tr("C64 EMULATION WIRD NUN GESTARTET."),Qt::darkBlue);

    LogText(tr(">> C64 Emulation wird gestartet.\n").toUtf8());

    if(ret_error == 0) c64->StartEmulation();
    else
    {
        LogText(tr("<< C64 Emulation konnte nicht gestartet werden.\n").toUtf8());
        this->close();
        return -1;
    }

    LogText(tr(">> C64 Emulation wurde gestartet.\n").toUtf8());

    c64->HardReset();

    LogText(tr(">> Hardreset wurde ausgefuehrt.\n").toUtf8());

    return 0;
}

void MainWindow::OnMessage(QStringList msg)
{
    if(c64 != nullptr)
		ExecuteCommandLine(msg);
}

void MainWindow::LogText(const char *log_text)
{
    mutex_log_text.lock();
    if(log != nullptr)
    {
        *log << log_text;
        log->flush();
    }
    mutex_log_text.unlock();
}

void MainWindow::CloseC64Screeen()
{
    on_actionBeenden_triggered();
}

void MainWindow::LimitCyclesEvent()
{
    IsLimitCyclesEvent = true;
    on_actionBeenden_triggered();
}

void MainWindow::DebugCartEvent(unsigned char value)
{
    DebugCartValue = value;
    IsDebugCartEvent = true;
    on_actionBeenden_triggered();
}

void MainWindow::SetCustomDataPath(QString path)
{
    custom_dataPath = path;
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

    if (event->type() == QEvent::Show)
    {
        qDebug("Show Event");
        c64->SetFocusToC64Window();
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

      if(lang == "German") lang = "Deutsch";

      QAction *action = new QAction(lang, this);
      action->setCheckable(true);
      action->setData(locale);

      QString iconfile = fileNames[i];
      iconfile.truncate(iconfile.lastIndexOf('.'));
      iconfile = dir.filePath(iconfile + ".png");
      action->setIcon(QIcon(iconfile));
      action->setIconVisibleInMenu(true);
      action->setStatusTip(tr("Wechseln zur Sprache: ") + lang);

      ui->menuSprache->addAction(action);
      langGroup->addAction(action);

      // set default translators and language checked
      if (defaultLocale == locale)
      {
         appTranslator.load("emu64_" + action->data().toString(), langPath);
         qtTranslator.load("qt_" + action->data().toString(), langPath);
         action->setChecked(true);
         ui->retranslateUi(this);
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

    for(int i=0; i<MAX_FLOPPY_NUM; i++)
    {
        WidgetFloppyStatus *w = (WidgetFloppyStatus*)ui->FloppyTabel->cellWidget(i,0);
        if(w != nullptr) w->RetranslateUi();
    }

    info_window->RetranslateUi();
    video_crt_setup_window->RetranslateUi();
    c64_keyboard_window->RetranslateUi();
    floppy_window->RetranslateUi();
    tape_window->RetranslateUi();
    cartridge_window->RetranslateUi();
    debugger_window->RetranslateUi();
    setup_window->RetranslateUi();
    speed_window->RetranslateUi();
    video_capture_window->RetranslateUi();
    oscilloscope_window->RetranslateUi();

    SetC64ScreenTitle();
}

void MainWindow::SetC64ScreenTitle()
{
    c64->SetSDLWindowName(tr("C64 Bildschirm").toLocal8Bit());
}

void MainWindow::ExecuteCommandLine(QStringList string_list)
{
	char** arg;
	int argc = string_list.length();

	arg = new char*[string_list.length()];

	for(int i=0; i<argc; i++)
	{
		arg[i] = new char[string_list.at(i).size()+1];
		strcpy(arg[i],string_list.at(i).toLatin1().data());
	}

	CommandLineClass *cmd_line = new CommandLineClass(argc, arg, "emu64",command_list, command_list_count);

    bool error;
    int lwnr,adr,val;
	QString filename;
    QFileInfo *fi;

    bool loop_break = false;

	if(cmd_line == nullptr) return;

    for(int i=0; i<cmd_line->GetCommandCount() && !loop_break; i++)
    {
        int akt_command = cmd_line->GetCommand(i);
        switch(akt_command)
        {
        case CMD_ARG:
            break;
        case CMD_AUTOSTART:
			AutoLoadAndRun(string_list[i+2]);
            break;
        case CMD_HARDRESET:
            c64->HardReset();
            break;
        case CMD_SOFTRESET:
            c64->SoftReset();
            break;
        case CMD_MOUNT_DISK:
            lwnr = cmd_line->GetArgInt(i+1, &error);
			filename = string_list[i+3];

            if(!error)
            {
                if(lwnr >= 8 && lwnr <= 11)
                {
                    fi = new QFileInfo(filename);
                    if(fi->exists())
                    {
                        cout << "Laufwerksnummer: " << lwnr << endl;
                        cout << "Disk Image: " << fi->fileName().toLocal8Bit().data() << endl;

                        if(floppy_window->SetDiskImage(lwnr-8, fi->absoluteFilePath()))
                        {
                            WidgetFloppyStatus *w = (WidgetFloppyStatus*)ui->FloppyTabel->cellWidget(lwnr-8,0);
                            w->SetEnableFloppy(true);
                        }
                    }
                    else
                        cout << "Die angebene Datei existiert nicht." << endl;
                }
                else
                    cmd_line->OutErrorMsg("Laufwerksnummer muss zwischen 8 und 11 liegen!","--help");
            }
            break;
        case CMD_MOUNT_CRT:
			filename = string_list[i+2];
            fi = new QFileInfo(filename);
            if(fi->exists())
            {
                if(cartridge_window->SetCartridgeImage(fi->absoluteFilePath()))
                {
                    cartridge_window->ConnectCrt();
                }
            }
            else
                cout << "Die angebene Datei existiert nicht." << endl;
			delete fi;
            break;
        case CMD_UMOUNT_CRT:
            cartridge_window->DisconnectCrt();
            break;
        case CMD_WARP_MODE:
            c64->EnableWarpMode(true);
            break;
        case CMD_POKE_64:
            adr = cmd_line->GetArgInt(i+1, &error);
            if(error) break;

            val = cmd_line->GetArgInt(i+2, &error);
            if(error) break;

            if(!(adr >= 0x0000 && adr <= 0xffff))
            {
                cmd_line->OutErrorMsg("Die Adresse muss zwischen (0)0x0000 und (65535)0xffff liegen!","--help");
                break;
            }

            if(!(val >= 0x00 && val <= 0xff))
            {
                cmd_line->OutErrorMsg("Der zu schreibende Wert muss zwischen 0(0x00) und 255(0xff) liegen!","--help");
                break;
            }

            c64->WriteC64Byte((unsigned short)adr,(unsigned char)val);
            break;
        case CMD_LIMITCYCLES:
            val = cmd_line->GetArgInt(i+1, &error);
            if(error) break;

            if(!(val > 0))
            {
                cmd_line->OutErrorMsg("Die Anzahl der Zyklen müssen groeßer als 0 sein.","--help");
                break;
            }
            c64->SetLimitCycles(val);
            break;

        case CMD_EXIT_SCREENSHOT:
            c64->SetExitScreenshot(cmd_line->GetArg(i+1));
            break;

        case CMD_DEBUG_CART:
            c64->SetEnableDebugCart(true);
            break;

        case CMD_VIDEOCAPTURE:
            cout << "Video Record: " << cmd_line->GetArg(i+1) << endl;
            c64->StartVideoRecord(cmd_line->GetArg(i+1));
            break;
        }

        // ----------------------------------------------------

        // i korregieren (abhängig von der Anzahl der Argumente)
        if(akt_command != CMD_ARG)
            i += cmd_line->GetCommandArgCount(akt_command);
    }
}

void MainWindow::SplashMessage(const QString &message, const QColor &color)
{
	if(splash != nullptr) splash->ShowStatusMessage(message,color);
}

void MainWindow::AutoLoadAndRun(QString filename)
{
	cartridge_window->DisconnectCrt();

	// akutelles Autostart Verzeichnis abspeichern
	QFileInfo file_info(filename);
	lastAutoloadPath = file_info.absolutePath();

	int typ = NO_C64_FILE;

	if(file_info.completeSuffix().toUpper() == "PRG")
		typ = PRG;

	if(file_info.completeSuffix().toUpper() == "C64")
		typ = C64;

	if(file_info.completeSuffix().toUpper() == "T64")
		typ = T64;

	if(file_info.completeSuffix().toUpper() == "P00")
		typ = P00;

	if(file_info.completeSuffix().toUpper() == "D64")
		typ = D64;

	if(file_info.completeSuffix().toUpper() == "G64")
		typ = G64;

	if(file_info.completeSuffix().toUpper() == "CRT")
		typ = CRT;

	if(file_info.completeSuffix().toUpper() == "FRZ")
		typ = FRZ;

	// QMessageBox::information(this,"Test",filename);

	FILE *file = qfopen(filename, "rb");

	if(c64->LoadAutoRun(0, file, filename.toLocal8Bit(), typ) == 0)
	{
		cout << "AUTOLOAD: " << typ << ", FILE: " << file << endl;

		// Prüfen welche
		if(typ == D64)
		{
			WidgetFloppyStatus *w = (WidgetFloppyStatus*)ui->FloppyTabel->cellWidget(0,0);
			w->SetEnableFloppy(true);
			floppy_window->SetDiskImage(0,filename);
		}
	}
}

void MainWindow::on_menu_main_info_triggered()
{
    SDL_version compiled;
    SDL_VERSION(&compiled);

    QString sdl_version = QVariant(compiled.major).toString() + "." + QVariant(compiled.minor).toString() + "." + QVariant(compiled.patch).toString();
    QString emu64_version = QString("Emu64 V") + QString(VERSION_STRING);

    info_window->SetEmu64VersionText(emu64_version);
    info_window->SetMoreInfoText("Qt Version: " + QString(qVersion()) + " / SDL Version: " + sdl_version + " / FFMpeg Version: " + QString(c64->GetAVVersion()));

    info_window->show();
}

void MainWindow::on_actionBeenden_triggered()
{
    if(!debugger_window->isHidden()) debugger_window->hide();
    showNormal();
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
    QDir auto_path = lastAutoloadPath;

    // Wenn das lestzte Autolad Verzeichnis nicht mehr existiert, wird das Homeverzeichnis gesetzt
    if(!auto_path.exists())
    {
        lastAutoloadPath = QDir::homePath();
    }

	QString filename = QFileDialog::getOpenFileName(this,tr("C64 Dateien öffnen "),lastAutoloadPath,tr("C64 Programm Dateien ") + "(*.prg *.c64 *.p00 *.t64 *.d64 *.g64 *.crt *.frz);;" + tr("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
		AutoLoadAndRun(filename);
        c64->SetFocusToC64Window();
    }
}

void MainWindow::on_actionC64_Programme_direkt_laden_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("C64 Dateien öffnen "),"",tr("C64 Programm Dateien ") + "(*.prg *.c64 *.p00 *.t64 *.frz);;" + tr("Alle Dateien ") + "(*.*)",0,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
		QFileInfo file_info(filename);

		int typ = NO_C64_FILE;
		if(file_info.completeSuffix().toUpper() == "PRG")
			typ = PRG;

		if(file_info.completeSuffix().toUpper() == "C64")
			typ = C64;

		if(file_info.completeSuffix().toUpper() == "T64")
			typ = T64;

		if(file_info.completeSuffix().toUpper() == "P00")
			typ = P00;

		FILE *file = qfopen(filename, "rb");

		c64->LoadPRG(file, filename.toLocal8Bit(), typ, nullptr);
    }
}

void MainWindow::on_actionVideo_Crt_Setup_triggered()
{
    video_crt_setup_window->show();
}

void MainWindow::on_actionFloppy_1541_II_triggered()
{
    if(floppy_window->isHidden()) floppy_window->show();
    else floppy_window->hide();
}

void MainWindow::on_actionVirtuelle_C64_Tastatur_triggered()
{
    c64_keyboard_window->recording = false;
    c64_keyboard_window->show();
    c64_keyboard_window->update();
}

void MainWindow::on_actionC64_Tastaturbelegung_ndern_triggered()
{
    c64_keyboard_window->recording = true;
    c64_keyboard_window->show();
    c64_keyboard_window->update();
}

void MainWindow::on_actionSteckmodule_triggered()
{
    if(cartridge_window->isHidden()) cartridge_window->show();
    else cartridge_window->hide();
}

void MainWindow::on_actionDebugger_Disassembler_triggered()
{
    if(debugger_window->isHidden()) debugger_window->show();
    else debugger_window->hide();
}

void MainWindow::on_actionEmu64_Einstellungen_triggered()
{
    if(setup_window->isHidden())
    {
        c64->JoystickNewScan();
        setup_window->show();
    }
    else setup_window->hide();
}

void MainWindow::OnChangeGrafikModi(bool fullscreen, bool palmode, bool doublemode, bool filter)
{
    if(!fullscreen)
    {
        if(c64 != nullptr) c64->SetGrafikModi(doublemode,palmode,filter,0,0);
        char out_text[1024];
        sprintf(out_text,">> Change Grafik Mode: %d-%d-%d\n",doublemode,palmode,filter);
        LogText(out_text);
    }
}

void MainWindow::OnChangeFloppyImage(int floppynr)
{
    WidgetFloppyStatus *w = (WidgetFloppyStatus*)ui->FloppyTabel->cellWidget(floppynr,0);
    w->SetAktFilename(floppy_window->GetAktFilename(floppynr),floppy_window->GetAktD64Name(floppynr));

	QFileInfo file_info(floppy_window->GetAktFilename(floppynr));

	int typ = NO_C64_FILE;
	if(file_info.completeSuffix().toUpper() == "D64")
		typ = D64;

	if(file_info.completeSuffix().toUpper() == "G64")
		typ = G64;

	FILE *file = qfopen(floppy_window->GetAktFilename(floppynr), "r+b");
	c64->LoadDiskImage(floppynr,file, typ);
}

void MainWindow::OnResetScreenshotCounter()
{
    c64->SetScreenshotNumber(0);
}

void MainWindow::OnSetupFished(int result)
{
    if(result == 0)
    {
        c64->StopRecJoystickMapping();
        c64->SetScreenshotFormat(setup_window->GetScreenshotFormat());
    }
}

void MainWindow::on_actionScreenshot_triggered()
{
    if(ScreenshotsEnable)
    {
        c64->SaveScreenshot();
    }
    else QMessageBox::critical(this,tr("Emu64 Fehler ..."),tr("Es sind keine Screenshots möglich da Emu64 kein Screenshot Verzeichnis anlegen konnte.\nÜberprüfen Sie bitte die Rechte des Emu64 Verzeichnisses !"));
}

void MainWindow::on_actionC64_Geschwindigkeit_triggered()
{
    if(speed_window->isHidden()) speed_window->show();
    else speed_window->hide();
}

/// REU Tools ///

void MainWindow::on_actionREU_einstecken_triggered()
{
    c64->InsertREU();
    c64->SetFocusToC64Window();
}

void MainWindow::on_actionREU_entfernen_triggered()
{
    c64->RemoveREU();
    c64->SetFocusToC64Window();
}

void MainWindow::on_actionREU_laden_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("REU Inhalt laden"),QDir::homePath(),tr("REU Image Dateien") + "(*.reu);;" + tr("Alle Dateien") + "(*.*)",nullptr,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
        if(c64->LoadREUImage(filename.toLocal8Bit()) != 0)
            QMessageBox::critical(this,tr("Emu64 Fehler ..."),tr("Beim laden des REU Images trat ein Fehler auf!"));
    }
}

void MainWindow::on_actionREU_speichern_triggered()
{
    QString filename;
    QString fileext;

    QStringList filters;
    filters << tr("REU Image Dateien (*.reu)")
            << tr("Alle Dateien (*.*)");

    if(!CustomSaveFileDialog::GetSaveFileName(this,tr("REU Inhalt speichern"), filters, &filename, &fileext))
        return;

    if(c64->SaveREUImage(filename.toLocal8Bit()) != 0)
        QMessageBox::critical(this,tr("Emu64 Fehler ..."),tr("Beim laden des REU Images trat ein Fehler auf!"));
}

void MainWindow::on_actionREU_loeschen_triggered()
{
    if(QMessageBox::Yes == QMessageBox::question(this,tr("REU Speicher löschen ..."),tr("Möchten Sie den Inhalt des REU Speichers wirklich löschen?"),QMessageBox::Yes | QMessageBox::No))
    {
        c64->ClearREURam();
    }
}

/// GEO-RAM Tools ///

void MainWindow::on_actionGEO_einstecken_triggered()
{
    c64->InsertGEORAM();
    c64->SetFocusToC64Window();
}

void MainWindow::on_actionGEO_entfernen_triggered()
{
    c64->RemoveGEORAM();
    c64->SetFocusToC64Window();
}

void MainWindow::on_actionGEO_laden_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("GEORAM Inhalt laden"),QDir::homePath(),tr("GEORAM Image Dateien") + "(*.img);;" + tr("Alle Dateien") + "(*.*)",nullptr,QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
        if(c64->LoadGEORAMImage(filename.toLocal8Bit()) != 0)
            QMessageBox::critical(this,tr("Emu64 Fehler ..."),tr("Beim laden des GEORAM Images trat ein Fehler auf!"));
    }
}

void MainWindow::on_actionGEO_speichern_triggered()
{
    QString filename;
    QString fileext;

    QStringList filters;
    filters << tr("GEORAM Image Dateien (*.img)")
            << tr("Alle Dateien (*.*)");

    if(!CustomSaveFileDialog::GetSaveFileName(this,tr("GEORAM Inhalt speichern"), filters, &filename, &fileext))
        return;

    if(c64->SaveGEORAMImage(filename.toLocal8Bit()) != 0)
        QMessageBox::critical(this,tr("Emu64 Fehler ..."),tr("Beim laden des GEORAM Images trat ein Fehler auf!"));
}

void MainWindow::on_actionGEO_loeschen_triggered()
{
    if(QMessageBox::Yes == QMessageBox::question(this,tr("GEORAM Speicher löschen ..."),tr("Möchten Sie den Inhalt des GEORAM Speichers wirklich löschen?"),QMessageBox::Yes | QMessageBox::No))
    {
        c64->ClearGEORAMRam();
    }
}

void MainWindow::on_actionVollbild_triggered()
{
    c64->SetFullscreen(true);
}

void MainWindow::on_actionBandlaufwerk_1530_triggered()
{
    if(tape_window->isHidden()) tape_window->show();
    else tape_window->hide();
}

void MainWindow::on_actionC64_Tastenbelegung_Show_triggered()
{
    show_c64keymap_window->UpdateText();
    show_c64keymap_window->show();
}

void MainWindow::on_actionVideo_Capture_triggered()
{
    video_capture_window->show();
}

void MainWindow::on_actionSID_Dump_triggered()
{
    sid_dump_window->show();
}

void MainWindow::on_actionAudio_Oszilloskop_triggered()
{
    oscilloscope_window->show();
}
