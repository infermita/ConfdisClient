#include "socketclient.h"
#include <QSettings>
#include "mainwindow.h"
#include <QDebug>


SocketClient::SocketClient()
{

}
QByteArray SocketClient::Connect(QString cmd){

    quint16 packet_size;

    QSettings settings(MainWindow::getInstance()->path+"/confdis.ini", QSettings::IniFormat);

    settings.beginGroup("application");
    QString ip = settings.value("ipServer","192.168.2.5").toString();
    int port = settings.value("ipServerPort","4096").toInt();

    qDebug() << "Connect IP server: " << ip << " at port: " << QString::number(port);

    socket = new QTcpSocket(this);
    socket->connectToHost(ip, port);

    QByteArray ret = "error";

    if(socket->waitForConnected(3000))
    {
        qDebug() << "Connected!";
        ret.clear();

        // send
        socket->write(cmd.toUtf8());
        socket->waitForBytesWritten(1000);
        //socket->waitForReadyRead(10000);

        while(1){

            socket->waitForReadyRead(5000);

            packet_size=socket->bytesAvailable();//Read Count Bytes waiting on line
            //qDebug()<<"Packet Recived len: "<<QString::number(packet_size)<<"\n";
            //rcv += packet_size;


            if (packet_size>0)
            {

                ret.append(socket->read(packet_size));//Read Packet
                //buffer.append(socket->readAll());
                //...
             }else{
                break;
            }
        }

        //qDebug() << "Reading: " << socket->bytesAvailable();

        //ret =  socket->readAll();

        //qDebug() << ret;

        socket->close();
    }
    else
    {
        qDebug() << "Not connected!";
        QUdpSocket *udpSocket = new QUdpSocket(this);
        QByteArray datagram = "{\"cmd\": \"ipserverrequest\"}";
        udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 4099);
        udpSocket->close();
    }

    return ret;

}
