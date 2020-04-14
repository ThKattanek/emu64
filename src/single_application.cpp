//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: single_application.cpp                //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 13.09.201A                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifdef _WIN32
#include <windows.h>
#include <lmcons.h>
#else
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <QByteArray>
#endif
#include <QCryptographicHash>
#include <QDataStream>
#include <QLocalSocket>

#include "single_application.h"

SingleApplication::SingleApplication(int &argc, char *argv[]) : QApplication(argc, argv)
{
    // calculate identifier for this instance,
    // using organization and application name
    QCryptographicHash appData(QCryptographicHash::Sha256);
    appData.addData(QCoreApplication::organizationName().toUtf8());
    appData.addData(QCoreApplication::applicationName().toUtf8());

#ifdef _WIN32
    // add current user to identifier on Windows
    wchar_t username[UNLEN + 1];
    DWORD usernameLen = UNLEN + 1;
    if (GetUserNameW(username, &usernameLen))
    {
	appData.addData(QString::fromWCharArray(username).toUtf8());
    }
    else
    {
	appData.addData(qgetenv("USERNAME"));
    }
#else
    // add current user to identifier on POSIX
    QByteArray username;
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw)
    {
	username = pw->pw_name;
    }
    if (username.isEmpty())
    {
	username = qgetenv("USER");
    }
    appData.addData(username);
#endif

    // calculate identifier string and create local socket
    instanceServerName = appData.result().toBase64().replace("/","_");
    bool listening = instanceServer.listen(instanceServerName);

#ifdef _WIN32
    // on Windows, multiple servers can listen on the same local socket,
    // so make sure we are the first one using a system-wide mutex.
    // This mutex is automatically destroyed on application exit, even in
    // case of a crash.
    if (listening)
    {
	CreateMutexW(0, true,
		reinterpret_cast<LPCWSTR>(instanceServerName.utf16()));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
	    instanceServer.close();
	    listening = false;
	}
    }
#else
    // on POSIX, only one server can listen on a local socket, but a crashing
    // application can leave behind the socket, so make sure it's still alive
    // by connecting to it -- otherwise, remove stale socket
    if (!listening)
    {
	QLocalSocket sock;
	sock.connectToServer(instanceServerName, QIODevice::WriteOnly);
	if (sock.state() != QLocalSocket::ConnectedState &&
		!sock.waitForConnected(2000))
	{
	    QLocalServer::removeServer(instanceServerName);
	    listening = instanceServer.listen(instanceServerName);
	}
	else
	{
	    sock.disconnectFromServer();
	}
    }
#endif

    bAlreadyExists = !listening;

    if (listening)
    {
	// if we're listening on the local socket (first running instance),
	// handle connections to it.
	connect(&instanceServer, &QLocalServer::newConnection, this, [this](){
		QLocalSocket *conn = instanceServer.nextPendingConnection();
		connect(conn, &QIODevice::readyRead, this, [this](){
			QLocalSocket *sock =
				qobject_cast<QLocalSocket *>(sender());
			if (sock)
			{
			    activeClients.insert(sock);
			    QDataStream recvStream(sock);
			    QStringList arguments;
			    QString argument;
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
			    for (;;)
			    {
				recvStream.startTransaction();
#else
			    while (!recvStream.atEnd())
			    {
#endif
				recvStream >> argument;
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
				if (!recvStream.commitTransaction()) break;
#endif
				arguments << argument;
			    }
			    activeClients.remove(sock);
			    if (sock->state() != QLocalSocket::ConnectedState)
			    {
				emit remoteActivated();
				sock->deleteLater();
			    }
			    if (arguments.size())
			    {
				emit messageAvailable( arguments );
			    }
			}
		    });
		connect(conn, &QLocalSocket::disconnected, this, [this](){
			QLocalSocket *sock =
				qobject_cast<QLocalSocket *>(sender());
			if (sock)
			{
			    if (!activeClients.contains(sock))
			    {
				emit remoteActivated();
				sock->deleteLater();
			    }
			}
		    });
		QDataStream sendStream(conn);
		sendStream << applicationPid();
		conn->flush();
	    });
    }
}

// public functions.
bool SingleApplication::sendMessages(const QStringList &messages)
{
    //we cannot send mess if we are master process!
    if (isMasterApp()){
        return false;
    }

    QLocalSocket sock;
    sock.connectToServer(instanceServerName);
    if (sock.state() == QLocalSocket::ConnectedState
	    || sock.waitForConnected(5000))
    {
	QDataStream stream(&sock);
	if (sock.waitForReadyRead(5000))
	{
	    qint64 mainpid;
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
	    stream.startTransaction();
#endif
	    stream >> mainpid;
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
#ifdef _WIN32
	    if (stream.commitTransaction())
	    {
		AllowSetForegroundWindow(DWORD(mainpid));
	    }
#else
	    stream.commitTransaction();
#endif
#else
#ifdef _WIN32
	    AllowSetForegroundWindow(DWORD(mainpid));
#endif
#endif
	}
	for (QStringList::const_iterator i = messages.constBegin();
		i != messages.constEnd(); ++i)
	{
	    stream << *i;
	}
	sock.flush();
	sock.disconnectFromServer();
	sock.state() == QLocalSocket::UnconnectedState ||
	    sock.waitForDisconnected(1000);
    }

    return true;
}
