#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QPalette>
#include <QPixmap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkInterface>
//#include "lib/nfcthread.h"
#include "lib/dao.h"
#include "lib/socketclient.h"
#include <wiringPi.h>
#include <QCryptographicHash>
#include <QProcess>
#include <QSettings>
#include <QFile>
#include "lib/ntpdate.h"
#include <unistd.h>

MainWindow* MainWindow::instance = 0;

MainWindow* MainWindow::getInstance(){
    if(instance==0){
        instance = new MainWindow;
    }
    return instance;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->call->hide();

    instance = this;
    nfc_status = "0";
    timerId = 0;

    numCheckserver = numCheckNtp = 1;
    idMonitor = 0;
    sessionid = "";

    pinOk=5;
    pinKo=4;

    if(QString(getenv("USER"))=="alberto"){
        path = qApp->applicationDirPath()+"/images";
    }else{
        path = "/etc/confdis";
    }

    SetImage("default",false);

    //Comando accensione spegnimeto HDMI su raspi
    //vcgencmd display_power 1
    //NfcThread *nfcTh = new NfcThread();
    //nfcTh->start();

    if(QString(getenv("USER"))!="alberto"){

        wiringPiSetup();
        pinMode (pinOk, OUTPUT) ;
        pinMode (pinKo, OUTPUT) ;

    }

    lT = new LedThread();

    QTimer::singleShot(100,this,SLOT(StartThGetId()));

    connect(this,SIGNAL(SigStartThCheckImage(QString,QString,QString)),this, SLOT(StartThCheckImage(QString,QString,QString)));


}

MainWindow::~MainWindow()
{
    qDebug() << "Esco";
    //if (pnd != NULL)
    //    nfc_abort_command(pnd);
    delete ui;
}

void MainWindow::SetImage(QString id,bool check){

    ui->version->hide();
    //ui->ip->hide();

    if(id=="default"){
        QPixmap imgD(path+"/defaultimg.jpg");
        ui->label->setPixmap(imgD);
        ui->label->setScaledContents( true );
        ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    }else if(id=="idMonitor"){
        if(idMonitor==0) {
            ui->label->setText("<font color='white'>ID: UNDEFINED</font>");
        }else{
            ui->label->setText("<font color='white'>ID: "+QString::number(idMonitor)+"</font>");
        }
        ui->version->setText("<font color='white'>ConfDis Ver. 1.7</font>");
        ui->version->show();
    }else if(id=="config"){
        ui->label->setText("<font color='white'>CONFIG MODE</font>");
    }else if(id=="error"){
        ui->version->setText("<font color='white'>SERVER ERROR</font>");
        ui->version->show();
    }else{
        Dao *d = new Dao();

        QHash<QString,QString> res;

        bool isNumber;

        id.toInt(&isNumber,10);

        if(isNumber){
             res = d->singleRow("images","id='"+id+"'");
        }else{
             res = d->singleRow("images","cardkey='"+id+"'");
        }
        delete d;

        if(check){

            if(res.count()){

                qDebug() << "Record per card id: " << id << " trovato mando update img";

                emit SigStartThCheckImage(res["id"],id,res["image"]);
            }else{

                qDebug() << "Record per card id: " << id << "non trovato mando update img";

                emit SigStartThCheckImage("0",id,"null");
            }

        }else{

            if(res.count()){
                QByteArray ba;

                qDebug() << "Record per card id: " << id << "trovato";

                ba.append(res["image"]);

                QPixmap img;//(path+"/images/executive.jpg");

                if(QByteArray::fromBase64(ba).length()){

                    img.loadFromData(QByteArray::fromBase64(ba));
                    //img = img.scaled(QSize(1680, 342), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    ui->label->setPixmap(img);
                    ui->label->setScaledContents( true );
                    ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

                    lT->max = 3;
                    qDebug() << "Mostro immagine";
                }else{
                    if(isNumber){
                        ui->version->setText("<font color='red'>No Image</font>");
                    }else{
                        ui->version->setText("<font color='red'>No Image on NFC</font>");
                    }
                    ui->version->show();
                    lT->max = 10;
                    qDebug() << "Mostro No Image on NFC";
                    QTimer::singleShot(10000,this,SLOT(StartThSetDefImage()));
                }


            }else{
                if(isNumber){
                    ui->version->setText("<font color='red'>Image not found</font>");
                }else{
                    ui->version->setText("<font color='red'>Card not recognized</font>");
                }
                ui->version->show();
                lT->max = 10;
                qDebug() << "Card not recognized";
                QTimer::singleShot(10000,this,SLOT(StartThSetDefImage()));

            }
            lT->start();
        }


    }

}
void MainWindow::Exit(){
    qApp->exit(0);
}

void MainWindow::StartThGetId(){


    //if(QNetworkInterface::interfaceFromName("wlan0").IsUp){

        ui->version->setText("<font color='red'><strong>Offline</strong></font>");
        ui->version->show();

        QString ifname = "wlan0";

        if(QString(getenv("USER"))=="alberto"){

            ifname = "wlp0s20f3";

        }


        bool conn = false;

        if(QNetworkInterface::interfaceFromName(ifname).addressEntries().length()){

            QString ip = QNetworkInterface::interfaceFromName(ifname).addressEntries().first().ip().toString();

            if(ip.split(".").count()==4){

                ui->ip->setText("<font color='red'><strong>"+QNetworkInterface::interfaceFromName(ifname).addressEntries().first().ip().toString()+"</strong></font>");
                ui->ip->show();
                conn = true;

            }else{

                ui->ip->setText("<font color='red'><strong>IP UNKNOWN</strong></font>");
                ui->ip->show();

            }

        }else{

            ui->ip->setText("<font color='red'><strong>IP UNKNOWN</strong></font>");
            ui->ip->show();

        }

        if(conn){

            QJsonObject jo;
            jo.insert("cmd","getid");
            jo.insert("mac",QNetworkInterface::interfaceFromName(ifname).hardwareAddress());
            qDebug() << "Invio " << QString(QJsonDocument(jo).toJson());
            SocketClient sc;
            QByteArray res;

            res = sc.Connect(QString(QJsonDocument(jo).toJson()));
            qDebug() << "Get id return" << res.mid(0,200);


            if(res!="error"){

                QJsonDocument jd = QJsonDocument::fromJson(res);

                if(jd.isObject()){

                    if(jd.object().value("id_station").toString()==""){

                        //if(numCheckserver<4){

                            QTimer::singleShot(10000,this,SLOT(StartThGetId()));
                            //numCheckserver++;
                        //}

                    }else{

                        ui->version->hide();

                        idMonitor = jd.object().value("id_station").toString().toInt();

                        QPixmap imgSave;
                        QByteArray imgArr;
                        imgArr.append(QJsonDocument::fromJson(res).object().value("image").toString());
                        imgSave.loadFromData(QByteArray::fromBase64(imgArr));
                        bool save = imgSave.save(path+"/defaultimg.jpg");

                        SetImage("default",false);

                        QTimer::singleShot(5000,this,SLOT(HideIp()));
                    }

                    qDebug() << "GETID: " << jd.object().value("id_station").toString();
                }else{
                    qDebug() << "No valid json " << res.mid(0,200);
                    QTimer::singleShot(10000,this,SLOT(StartThGetId()));

                }

            }else{

                //if(numCheckserver<4){

                    QTimer::singleShot(10000,this,SLOT(StartThGetId()));
                    //numCheckserver++;
                //}

            }

        }else{
            QTimer::singleShot(1000,this,SLOT(StartThGetId()));
            qDebug() << "Intercaccia non attiva riprovo";
        }

}
void MainWindow::StartThCheckImage(QString id,QString cardKey,QString img){

    QJsonObject jo;

    if(sessionid!=""){

        jo.insert("cmd","nfcconfig");
        jo.insert("sessionid",sessionid);
        jo.insert("cardkey",cardKey);

    }else{

        jo.insert("cardkey",cardKey);
        jo.insert("cmd","slidenfc");
        QString md5Img = QString(QCryptographicHash::hash((img.toUtf8()),QCryptographicHash::Md5).toHex());
        jo.insert("md5_image",md5Img);
        jo.insert("id_image",id);
        jo.insert("id_station",idMonitor);

    }

    qDebug() << "Invio" << QString(QJsonDocument(jo).toJson());

    SocketClient sc;
    QByteArray res = sc.Connect(QString(QJsonDocument(jo).toJson()));
    if(res!="error"){

        QJsonParseError *error = new QJsonParseError();
        QJsonDocument d = QJsonDocument::fromJson(res,error);
        QJsonObject jObj = d.object();

        if(error->error==QJsonParseError::NoError){

            qDebug() << "Riveco cmd: " <<  jObj["cmd"].toString() << " Card: " << jObj["id_image"].toString();

            if(jObj["cmd"].toString()=="ko slidenfc"){

                SendImage("",cardKey);

            }else if(sessionid==""){
                SetImage(cardKey,false);
            }else{
                //sessionid = "";
                //SetImage("default",false);
                lT->max = 4;
                lT->start();
            }
        }else{
            emit SigStartThCheckImage(id,cardKey,img);
        }

    }


}
QString MainWindow::GetText(){

    return ui->label->text();

}
void MainWindow::StartThSetDefImage(){
    //SetImage("default",false);
    ui->version->hide();
}
void MainWindow::Ntpdate() {

    QSettings settings(path+"/confdis.ini", QSettings::IniFormat);

    settings.beginGroup("application");

    qDebug() << "Connect IP ntp: " << settings.value("ipServer","192.168.2.5").toString() << " at port: 123";

    if(NtpDate::SetDate(settings.value("ipServer","192.168.2.5").toString().toUtf8().data())){
        qDebug() << "Data settata";
    }else{
        qDebug() << "Data non settata";
        if(numCheckNtp<4){

            QTimer::singleShot(5000,this,SLOT(Ntpdate()));
            numCheckNtp++;
        }

    }

}
void MainWindow::SendImage(QString idImage,QString cardKey){

    SocketClient sc;
    int tent = 3;

    retrai:

        QJsonObject jo;
        jo.insert("cmd","sendimage");
        if(cardKey==""){
            jo.insert("id_image",idImage);
        }else{
            jo.insert("cardkey",cardKey);
        }
        jo.insert("id_station",MainWindow::getInstance()->idMonitor);

        qDebug() << "Invio" << QString(QJsonDocument(jo).toJson());

        QByteArray res = sc.Connect(QString(QJsonDocument(jo).toJson()));
        if(res!="error"){

            QJsonParseError *error = new QJsonParseError();
            QJsonDocument d = QJsonDocument::fromJson(res,error);
            QJsonObject jObj = d.object();

            if(error->error==QJsonParseError::NoError){

                qDebug() << "Ricevo da sendimage id image:" << jObj["id_image"].toString() << " Card key" << jObj["cardkey"].toString();

                if(jObj["id_image"].toString().length()){

                    QHash<QString,QString> field;
                    field.insert("id",jObj["id_image"].toString());
                    field.insert("image",jObj["image"].toString());
                    field.insert("cardkey",jObj["cardkey"].toString());
                    Dao *dao = new Dao();
                    dao->replaceRow("images",field);
                    delete dao;
                }

                if(cardKey==""){
                    SetImage(idImage,false);
                }else{
                    SetImage(cardKey,false);
                }
            }else{
                qDebug() << "Riprovo sendimage perchè ho ricevuto pacchetto json non corretto";
                if(cardKey==""){
                    SendImage(idImage,"");
                }else{
                    SendImage("",cardKey);
                }
            }



    }else{
        if(tent>0){
            tent--;
            sleep(10);
            qDebug() << "Riprovo sendimage perchè non ho ricevuto risposta dal server";
            goto retrai;
        }else{
            qDebug() << "Metto server error";
            SetImage("error",false);
        }
    }

}
void MainWindow::HideIp(){

    ui->ip->hide();

}
void MainWindow::timerEvent(QTimerEvent *event)
{
    qDebug() << "Entro in timer";
    if(ui->call->isHidden()){
        ui->call->show();
        qDebug() << "show";
    }else{
        ui->call->hide();
        qDebug() << "hide";
    }
}
void MainWindow::SetCall(QString status){

    if(timerId > 0)
        killTimer(timerId);

    if(status=="active"){

        ui->call->setStyleSheet("background-color:red;border-radius:10px;");
        ui->call->show();

    }else if(status=="request"){

        ui->call->setStyleSheet("background-color:green;border-radius:10px;");
        ui->call->show();

    }else if(status=="waiting"){

        ui->call->setStyleSheet("background-color:green;border-radius:10px;");
        ui->call->show();
        timerId = startTimer(500);

    }else{
        ui->call->hide();
        if(timerId > 0)
            killTimer(timerId);
    }



}

