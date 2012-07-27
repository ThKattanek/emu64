//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: main.cpp                              //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 27.07.2012		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include <QtGui/QApplication>
#include <QMessageBox>
#include "main_window.h"
#include "version.h"

#ifdef _WIN32
int qMain(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    QTextStream *log = 0;
    QFile LogFile("emu64.log");

    QApplication a(argc, argv);

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

    /*
    if(log!=0) *log << "comandline count: " << argc << "\n";
    for(int i=0;i<argc;i++)
        if(log!=0) *log << "comand [" << i << "] " << argv[i];
    */

    MainWindow w(0,log);

    w.log = log;
    w.show();

    return a.exec();
};
