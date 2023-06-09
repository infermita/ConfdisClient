#ifndef JSONCOMMAND_H
#define JSONCOMMAND_H

#include <QObject>
#include <QTcpSocket>
#include "lib/ledthread.h"

class JsonCommand
{
public:
    JsonCommand();
    void ElaborateCommand(QTcpSocket *socket);

private:
    LedThread *lT;
    void SendResponse(QTcpSocket *socket,QString response,QString addP="");
    QStringList options;
    void ChangeSet(QString ip,QString img);
    void ChangeIp(QString ip);


};

#endif // JSONCOMMAND_H
