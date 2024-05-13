#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QHash>
#include <QCryptographicHash>
#include <QSettings>
#include "lib/dao.h"
#include "jsoncommand.h"
#include "mainwindow.h"
#include "lib/socketclient.h"

JsonCommand::JsonCommand()
{
    lT = MainWindow::getInstance()->lT;
    options << "halt" << "reboot" << "hdmi off" << "hdmi on" << "images" << "reset" << "restart" << "ledok" << "lederr" << "whois" << "alive" << "config" << "getid" << "showimage" << "forceimage" << "changeset" << "speech" << "changeipserver";

}
void JsonCommand::ElaborateCommand(QTcpSocket *socket){

    QProcess p;
    QJsonArray listUsers;
    Dao *dao;
    QHash<QString,QString> field;
    QString read;



    QByteArray buffer;
    quint16 packet_size;
    int rcv = 0;


    while(1){

        socket->waitForReadyRead(1000);

        packet_size=socket->bytesAvailable();//Read Count Bytes waiting on line
        //qDebug()<<"Packet Recived len: "<<QString::number(packet_size)<<"\n";
        //rcv += packet_size;


        if (packet_size>0)
        {

            buffer.append(socket->read(packet_size));//Read Packet
            //buffer.append(socket->readAll());
            //...
         }else{
            break;
        }
    }
    //qDebug()<<"Packet Recived len: "<<QString::number(rcv)<<"\n";

    QJsonParseError *error = new QJsonParseError();
    QJsonDocument d = QJsonDocument::fromJson(QString(buffer).toUtf8(),error);
    QJsonObject jObj = d.object();
    QString cmd = jObj["cmd"].toString();

    if(cmd!="alive")
        qDebug() << "Leggo da socket: " << QString(buffer);

    //qDebug() << "Leggo Json: " << cmd;

    if(MainWindow::getInstance()->sessionid!="" && cmd!="restart"){
        SendResponse(socket,"ok "+cmd);

    }else{

        switch(options.indexOf(cmd)){

            case 0:
                //System halt
                SendResponse(socket,"ok "+cmd);
                p.start("halt");
                p.waitForFinished();
                p.close();
                break;
            case 1:
                //System reboot
                SendResponse(socket,"ok "+cmd);
                p.start("reboot");
                p.waitForFinished();
                p.close();
                break;
            case 2:
                //System hdmi off
                SendResponse(socket,"ok "+cmd);
                p.start("vcgencmd display_power 0");
                p.waitForFinished();
                p.close();
                MainWindow::getInstance()->hide();
                break;
            case 3:
                //System hdmi on
                SendResponse(socket,"ok "+cmd);
                p.start("vcgencmd display_power 1");
                p.waitForFinished();
                p.close();
                MainWindow::getInstance()->show();
                break;
            case 4:
                //Images
                SendResponse(socket,"ok "+cmd);
                listUsers = jObj["list"].toArray();
                dao = new Dao();
                foreach (const QJsonValue & users, listUsers) {

                    field.clear();

                    QJsonObject o = users.toObject();
                    qDebug() << "Id user: " << o["id"].toString();
                    field.insert("id",o["id"].toString());
                    field.insert("image",o["image"].toString());
                    field.insert("cardkey",o["cardkey"].toString());

                    if(dao->replaceRow("images",field)){
                        //qDebug() << "Ok insert: " << QString::number(o["id"].toInt());
                    }else{
                        //qDebug() << "Error insert: " << QString::number(o["id"].toInt());
                    }

                }
                delete dao;
            break;
            case 5:
                //default img
                SendResponse(socket,"ok "+cmd);
                MainWindow::getInstance()->SetImage("default",false);
                break;
            case 6:
                //restart
                SendResponse(socket,"ok "+cmd);
                //MainWindow::getInstance()->Exit();
                p.startDetached("/etc/init.d/confdis restart");
                p.close();
                break;
            case 7:
                //led ok
                SendResponse(socket,"ok "+cmd);
                lT->max = 3;
                lT->start();
                break;
            case 8:
                //led err
                SendResponse(socket,"ok "+cmd);
                lT->max = 10;
                lT->start();
                break;
            case 9:
                //whois
                SendResponse(socket,"ok "+cmd);
                MainWindow::getInstance()->SetImage("idMonitor",false);
                break;
            case 10:
                ChangeIp(jObj["serverip"].toString());
                //alive
                //p.start("vcgencmd display_power");
                //p.waitForFinished();
                //read = p.readAllStandardOutput().trimmed();
                //read = ",\""+read.replace("=","\":\"")+"\"";
                if(MainWindow::getInstance()->isHidden()){
                    read = ",\"display_power\":\"0\"";
                }else{
                    read = ",\"display_power\":\"1\"";
                }


                p.start("vcgencmd get_lcd_info");
                p.waitForFinished();
                if(p.readAllStandardOutput().trimmed()=="1680 1050 24"){
                    read += ",\"lcd_info\":\"1\"";
                }else{
                    read += ",\"lcd_info\":\"0\"";
                }
                read += ",\"nfc_status\":\""+MainWindow::getInstance()->nfc_status+"\"";
                p.close();
                SendResponse(socket,"ok "+cmd,read);
                break;
            case 11:
                //config
                SendResponse(socket,"ok "+cmd);
                MainWindow::getInstance()->sessionid = jObj["sessionid"].toString();
                MainWindow::getInstance()->SetImage("config",false);
                break;
            case 12:
                //get id
                SendResponse(socket,"ok "+cmd);
                MainWindow::getInstance()->idMonitor = jObj["id"].toString().toInt();
                break;
            case 13:
                //showimage
                ChangeIp(jObj["serverip"].toString());
                dao = new Dao();
                field = dao->singleRow("images","id='"+jObj["id_image"].toString()+"'");
                delete dao;
                SendResponse(socket,"ok "+cmd);
                if(field.count()){
                    if(QString(QCryptographicHash::hash((field["image"].toUtf8()),QCryptographicHash::Md5).toHex())==jObj["md5_image"].toString()){

                        MainWindow::getInstance()->SetImage(jObj["id_image"].toString(),false);
                    }else{
                        //SendResponse(socket,"ko "+cmd);
                        MainWindow::getInstance()->SendImage(jObj["id_image"].toString(),"");
                    }
                }else{
                    //SendResponse(socket,"ko "+cmd);
                    MainWindow::getInstance()->SendImage(jObj["id_image"].toString(),"");
                }

                break;
            case 14:
                //forceimage
                dao = new Dao();
                field.clear();

                field.insert("id",jObj["id_image"].toString());
                field.insert("image",jObj["image"].toString());
                field.insert("cardkey",jObj["cardkey"].toString());

                if(dao->replaceRow("images",field)){
                    SendResponse(socket,"ok "+cmd);
                }else{
                    SendResponse(socket,"ko "+cmd);
                }
                delete dao;
                break;
            case 15:
                //changeset
                SendResponse(socket,"ok "+cmd);
                ChangeSet(jObj["ip"].toString(),jObj["image"].toString());
                break;
            case 16:
                //speech
                SendResponse(socket,"ok "+cmd);
                MainWindow::getInstance()->SetCall(jObj["status"].toString());
                break;
            case 17:
                //changeipserver
                SendResponse(socket,"ok "+cmd);
                ChangeIp(jObj["serverip"].toString());
            default:
                SendResponse(socket,"error "+cmd);
                break;

        }
    }

}
void JsonCommand::SendResponse(QTcpSocket *socket,QString response,QString addP){

    if(addP!=""){

        response = "{\"cmd\":\""+response+"\""+addP+"}";

    }else{

        response = "{\"cmd\":\""+response+"\"}";

    }

    socket->write(response.toUtf8());
    socket->flush();
    socket->waitForBytesWritten(3000);

    socket->close();

}
void JsonCommand::ChangeSet(QString ip, QString img){

    QPixmap imgSave;
    QByteArray imgArr;
    imgArr.append(img);
    imgSave.loadFromData(QByteArray::fromBase64(imgArr));
    imgSave.save(MainWindow::getInstance()->path+"/defaultimg.jpg");

    QSettings settings(MainWindow::getInstance()->path+"/confdis.ini", QSettings::IniFormat);
    settings.beginGroup("application");
    settings.setValue("ipServer",ip);
    settings.endGroup();
    settings.sync();

    //MainWindow::getInstance()->SetImage("default",false);
    QProcess::startDetached("/etc/init.d/confdis restart");

}
void JsonCommand::ChangeIp(QString ip){

    QSettings settings(MainWindow::getInstance()->path+"/confdis.ini", QSettings::IniFormat);
    settings.beginGroup("application");

    QString oldIp = settings.value("ipServer","192.168.2.5").toString();

    if( oldIp!=ip){

        settings.setValue("ipServer",ip);
        settings.setValue("ipServerPort",4096);
        settings.endGroup();
        settings.sync();
        qDebug() << "Cambio IP Server: " << ip << "Vecchio ip " << oldIp;
    }
    //QProcess p;
    //p.startDetached("/etc/init.d/confdis restart");

}


