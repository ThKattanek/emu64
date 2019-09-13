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

#include <QTimer>
#include <QByteArray>

#include "single_application.h"

SingleApplication::SingleApplication(int &argc, char *argv[], const QString uniqueKey) : QApplication(argc, argv)
{
    sharedMemory = new QSharedMemory(this);

    sharedMemory->setKey(uniqueKey);

    // when  can create it only if it doesn't exist
    if (sharedMemory->create(5000))
    {
        sharedMemory->lock();
        *(char*)sharedMemory->data() = '\0';
        sharedMemory->unlock();

        bAlreadyExists = false;

        // start checking for messages of other instances.
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(checkForMessage()));
        timer->start(20);
    }
    // it exits, so we can attach it?!
    else if (sharedMemory->attach()){
        bAlreadyExists = true;
    }
    else{
        // error
    }
}

void SingleApplication::deleteSharedMemory()
{
    sharedMemory->detach();
    delete sharedMemory;
}


// public slots.

void SingleApplication::checkForMessage()
{
    QStringList arguments;

    sharedMemory->lock();
    char *from = (char*)sharedMemory->data();

    while(*from != '\0'){
        int sizeToRead = int(*from);
        ++from;

        QByteArray byteArray = QByteArray(from, sizeToRead);
        byteArray[sizeToRead] = '\0';
        from += sizeToRead;

        arguments << QString::fromUtf8(byteArray.constData());
    }

    *(char*)sharedMemory->data() = '\0';
    sharedMemory->unlock();

    if(arguments.size()) emit messageAvailable( arguments );
}

// public functions.

bool SingleApplication::sendMessage(const QString &message)
{
    //we cannot send mess if we are master process!
    if (isMasterApp()){
        return false;
    }

    QByteArray byteArray;
    byteArray.append(char(message.size()));
    byteArray.append(message.toLocal8Bit());
    byteArray.append('\0');

    //sharedMemory->lock();
    char *to = (char*)sharedMemory->data();
    while(*to != '\0'){
        int sizeToRead = int(*to);
        to += sizeToRead + 1;
    }

    const char *from = byteArray.data();
    memcpy(to, from, qMin(sharedMemory->size(), byteArray.size()));
    sharedMemory->unlock();

    return true;
}
