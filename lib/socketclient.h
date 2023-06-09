#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include "lib/ledthread.h"

class SocketClient : public QObject
{
    Q_OBJECT

public:
    SocketClient();
    QByteArray Connect(QString cmd);
private:
    QTcpSocket *socket;
    LedThread *lT;

};

#endif // SOCKETCLIENT_H
