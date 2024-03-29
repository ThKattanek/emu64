//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: main.cpp                              //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 20.03.2022                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "./single_application.h"
#include "./main_window.h"

#include "./custom_splashscreen.h"
#include <QBitmap>

#include "./command_line_class.h"   // Klasse zur Auswertung der KomandLine
#include "./emu64_commands.h"       // Struktur mit allen verfügbaren Kommandos

#if (!defined(SINGLE_THREADED_PLAYBACK) and defined(Q_WS_X11))
#include <X11/Xlib.h>
#endif

#undef main

int main(int argc, char *argv[])
{
#if (!defined(SINGLE_THREADED_PLAYBACK) and defined(Q_WS_X11))
    XInitThreads();
#endif

    QCoreApplication::setOrganizationName("ThKattanek");
    QCoreApplication::setApplicationName("Emu64");
    QCoreApplication::setApplicationVersion(VERSION_STRING);

    QTextStream *log = nullptr;
    QDir config_dir = QDir(QDir::homePath() + "/.config/emu64");

    bool start_minimized = false;
	bool enable_reu = false;
	bool enable_georam = false;

    CommandLineClass *cmd_line = new CommandLineClass(argc, argv, "emu64",command_list, command_list_count);

    // Prüfen ob ein Fehler bei der Kommandozeilenauswertung auftrat
    // Wenn ja emu64 beenden. (Fehlerausgabe kommt von CommandLineClass)
    if(cmd_line->GetCommandCount() < 0)
    {
        printf("\"emu64 --help\" liefert weitere Informationen.\n");
        return(-1);
    }

    if(cmd_line->GetCommandCount() > 0)
    {
        for(int i=0; i < cmd_line->GetCommandCount(); i++)
        {
            if(cmd_line->GetCommand(i) == CMD_RESET_INI)
            {
                QFile *config_file = new QFile(config_dir.path() + "/emu64.ini");
                if(!config_file->exists())
                {
                    std::cout << "emu64.ini existiert nicht, muss deshalb nicht geloescht werden." << std::endl;
                }
                else
                    if(config_file->remove())
                        std::cout << "emu64.ini wurde geloescht." << std::endl;
                    else
                    {
                        std::cout << "emu64.ini konnte nicht geloescht werden." << std::endl;
                        return(-1);
                    }
            }

            if(cmd_line->GetCommand(i) == CMD_MINIMIZED)
                start_minimized = true;

			if(cmd_line->GetCommand(i) == CMD_ENABLE_GEORAM)
				enable_georam = true;

			if(cmd_line->GetCommand(i) == CMD_ENABLE_REU)
				enable_reu = true;
        }

        if(cmd_line->GetCommand(0) == CMD_HELP)
        {
            cmd_line->ShowHelp();
            return(0x0);
        }

        if(cmd_line->GetCommand(0) == CMD_VERSION)
        {
            printf("Version: %s\n\n",VERSION_STRING);
            return(0x0);
        }
    }

	if(enable_georam == true && enable_reu == true)
	{
        std::cout << "Es können nicht '--enable-georam' und '--enable-reu' gleichzeitig gesetzt werden." << std::endl;
		return(-1);
	}

    SingleApplication *app;
    app = new SingleApplication (argc, argv);

    bool isFirstInstance;

    if(!cmd_line->FoundCommand(CMD_MULTIPLE_INSTANCE))
    {
        if (app->alreadyExists())
        {
	    QStringList args;
            for(int i=0;i<argc;i++) args << argv[i];
	    app->sendMessages(args);
            isFirstInstance = false;
            return 0;
        }
        else
        {
             isFirstInstance = true;
        }
    }
    else
    {
        isFirstInstance = true;
    }

#ifdef _WIN32
    FreeConsole();
    QFile LogFile("emu64.log");
#else
    if(!config_dir.exists())
    {
        QDir dir = QDir::root();
        dir.mkdir(config_dir.path());

        if(!config_dir.exists())
        {
            qDebug("Fatal Error ... not created emu64 config directory !!!");
            return app->exec();
        }
    }
    QFile LogFile(config_dir.path() + "/emu64.log");
#endif

    if(LogFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log = new QTextStream(&LogFile);
    }

#ifdef _WIN32
    if(log) *log << "*** Emu64 Win32 Binary File ***\n\n";
#else
# ifdef __linux__
    if(log) *log << "*** Emu64 Linux Binary File ***\n\n";
# else
#  ifdef __FreeBSD__
    if(log) *log << "*** Emu64 FreeBSD Binary File ***\n\n";
#  else
    if(log) *log << "*** Emu64 POSIX (unknown) Binary File ***\n\n";
#  endif
# endif
#endif

    if(log != nullptr) *log << "Emu64 Version: " << VERSION_STRING << "\n\n";

    MainWindow *w;

	if(!cmd_line->FoundCommand(CMD_NOSPLASH) && !cmd_line->FoundCommand(CMD_NOGUI))
    {
        QPixmap image(":/splash");
        CustomSplashScreen *splash = new CustomSplashScreen(image);
        splash->setMask(image.mask());
        splash->setWindowFlag(Qt :: WindowStaysOnTopHint);
        splash->show();

        w = new MainWindow(nullptr, splash, log);
    }
    else
    {
        w = new MainWindow(nullptr, nullptr, log);
    }

    w->start_minimized = start_minimized;

    // Wenn --minimized
	if(start_minimized && !cmd_line->FoundCommand(CMD_NOGUI))
    {
        w->showMinimized();
    }

    QObject::connect(app,SIGNAL(messageAvailable(QStringList)),w,SLOT(OnMessage(QStringList)));

    w->SetCustomDataPath("");

    // Der erste Parameter "--data-path" ist der prioresierte alle nachfolgenden zuweisungen werden ignoriert
    // Kommt in der regel nicht vor, wird aber für das linux appimage benötigt, da dort im .AppRun
    // --data-path zwingend übergeben werden muss.

    bool data_path_found = false;
    for(int i=0; i<cmd_line->GetCommandCount(); i++)
    {
        if((cmd_line->GetCommand(i) == CMD_DATA_PATH) && (!data_path_found))
        {
            w->SetCustomDataPath(QString(cmd_line->GetArg(i+1)));
            data_path_found = true;
        }
    }

    int ret = 0;

    w->log = log;
    w->no_write_ini_exit = true;

	if(w->OnInit(cmd_line->FoundCommand(CMD_NOGUI)) == 0)
    {
        w->no_write_ini_exit = false;
        if(isFirstInstance)
        {
            QStringList msg_list;
            for(int i=0;i<argc;i++) msg_list << argv[i];
            w->OnMessage(msg_list);
        }

        ret = app->exec();
    }

    if(w->IsLimitCyclesEvent) ret = 1;
    if(w->IsDebugCartEvent) ret = w->DebugCartValue;

    delete w;
    delete app;

    std::cout << "ExitCode: 0x" << std::hex << ret << std::endl;
    return ret;
};
