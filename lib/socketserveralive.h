#ifndef SOCKETSERVERALIVE_H
#define SOCKETSERVERALIVE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class SocketServerAlive : public QObject
{
    Q_OBJECT

public:
    explicit SocketServerAlive(QObject *parent = 0);

public slots:
    void ReadData();

private:
    QTcpServer *server;

};

#endif // SOCKETSERVERALIVE_H
