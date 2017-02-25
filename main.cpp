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
// Letzte Änderung am 25.02.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////


#include "./single_application.h"
#include "./main_window.h"

#include "./custom_splashscreen.h"
#include <QBitmap>
#include <QTimer>

#include "./command_line_class.h"   // Klasse zur Auswertung der KomandLine
#include "./emu64_commands.h"       // Struktur mit allen verfügbaren Kommandos

#undef main
int main(int argc, char *argv[])
{
    QTextStream *log = 0;
    QDir config_dir = QDir(QDir::homePath() + "/.config/emu64");

    CommandLineClass *cmd_line = new CommandLineClass(argc, argv, "emu64",command_list, command_list_count);

    // Prüfen ob ein Fehler bei der Kommandozeilenauswertung auftrat
    // Wenn ja emu64 beenden. (Fehlerausgabe kommt von CommandLineClass)
    if(cmd_line->GetCommandCount() < 0)
    {
        printf("\"emu64 --help\" liefert weitere Informationen.\n");
        return(0x0);
    }

    if(cmd_line->GetCommandCount() > 0)
    {
        if(cmd_line->GetCommand(0) == CMD_HELP)
        {
            cmd_line->ShowHelp();
            return(0x0);
        }
        if(cmd_line->GetCommand(0) == CMD_VERSION)
        {
            printf("Version: %s\n\n",str_emu64_version);
            return(0x0);
        }
    }

    SingleApplication *app;
    app = new SingleApplication (argc, argv,  "Emu64_By_Thorsten_Kattanek");

    //app->deleteSharedMemory();

    if (app->alreadyExists())
    {
        for(int i=0;i<argc;i++) app->sendMessage(argv[i]);
        return 0;
    }

#ifdef _WIN32
    QFile LogFile("emu64.log");
#endif

#ifdef __linux__
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
#endif

#ifdef __linux__
    if(log) *log << "*** Emu64 Linux Binary File ***\n\n";
#endif

    if(log!=0) *log << "Emu64 Version: " << str_emu64_version << "\n\n";


    QPixmap image(":/splash");
    customSplashScreen *splash = new customSplashScreen(image);
    splash->setPixmap(image);
    splash->setMask(image.mask());
    splash->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::SplashScreen);

    splash->show();


    MainWindow *w;
    w = new MainWindow(0,splash,log);
    //w = new MainWindow(0,NULL,log);

    QObject::connect(app,SIGNAL(messageAvailable(QStringList)),w,SLOT(OnMessage(QStringList)));

    w->log = log;

    QStringList msg_list;
    for(int i=0;i<argc;i++) msg_list << argv[i];
    w->OnMessage(msg_list);

    w->OnInit();

    int ret = app->exec();

    app->deleteSharedMemory();
    delete w;

    cout << "ExitCode: 0x" << std::hex << ret << endl;
    return ret;
};
