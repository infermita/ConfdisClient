#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include "lib/ledthread.h"

class SocketServer : public QObject
{
    Q_OBJECT

public:
    explicit SocketServer(QObject *parent = 0);

public slots:
    void ReadData();

private:
    QTcpServer *server;

};

#endif // SOCKETSERVER_H
