#include "socketserver.h"
#include "lib/jsoncommand.h"

SocketServer::SocketServer(QObject *parent) :
    QObject(parent)
{

    server = new QTcpServer(this);

    connect(server, SIGNAL(newConnection()), this, SLOT(ReadData()));

    if(!server->listen(QHostAddress::Any, 4096))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!";

    }


}

void SocketServer::ReadData(){

    QTcpSocket *socket = server->nextPendingConnection();

    JsonCommand jo;
    jo.ElaborateCommand(socket);

}

