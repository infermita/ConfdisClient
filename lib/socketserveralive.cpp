#include "socketserveralive.h"
#include "jsoncommand.h"

SocketServerAlive::SocketServerAlive(QObject *parent) :
    QObject(parent)
{

    server = new QTcpServer(this);

    connect(server, SIGNAL(newConnection()), this, SLOT(ReadData()));

    if(!server->listen(QHostAddress::Any, 4097))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!";

    }


}

void SocketServerAlive::ReadData(){

    QTcpSocket *socket = server->nextPendingConnection();

    JsonCommand jo;
    jo.ElaborateCommand(socket);

}
