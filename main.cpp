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
// Letzte Änderung am 15.02.2013                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "single_application.h"
#include "main_window.h"
#include "version.h"

#include <QSplashScreen>
#include <QBitmap>
#include <QTimer>

#ifdef _WIN32
int qMain(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    QTextStream *log = 0;
    QFile LogFile("emu64.log");

    SingleApplication app(argc, argv,  "Emu64_By_Thorsten_Kattanek");
    if (app.alreadyExists())
    {
        for(int i=0;i<argc;i++) app.sendMessage(argv[i]);
        return 0;
    }

    QPixmap pixmap(":/splash");
    QSplashScreen *splash = new QSplashScreen();
    splash->setPixmap(pixmap);
    splash->setMask(pixmap.mask());
    splash->show();

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

    MainWindow w(0,splash,log);

    QObject::connect(&app,SIGNAL(messageAvailable(QStringList)),&w,SLOT(OnMessage(QStringList)));

    w.log = log;

    QStringList msg_list;
    for(int i=0;i<argc;i++)
    {
        msg_list << argv[i];
    }

    w.OnMessage(msg_list);

    QTimer::singleShot(500, &w, SLOT(OnInit()));

    return app.exec();
};
