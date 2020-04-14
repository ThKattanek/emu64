//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: single_application.h                  //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 15.05.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef SINGLE_APPLICATION_H
#define SINGLE_APPLICATION_H

#include <QApplication>
#include <QLocalServer>
#include <QSet>
#include <QString>

class SingleApplication : public QApplication
{
        Q_OBJECT
public:
        SingleApplication(int &argc, char *argv[]);

        bool alreadyExists() const{
            return bAlreadyExists;
        }
        bool isMasterApp() const{
            return !alreadyExists();
        }

        bool sendMessages(const QStringList &messages);

signals:
        void messageAvailable(const QStringList& messages);
	void remoteActivated();

private:
	QString instanceServerName;
	QLocalServer instanceServer;
	QSet<QLocalSocket *> activeClients;
        bool bAlreadyExists;
};
#endif // SINGLE_APPLICATION_H
