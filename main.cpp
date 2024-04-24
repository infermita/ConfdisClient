#include "mainwindow.h"
#include <QApplication>
#include <signal.h>
#include <unistd.h>
#include <QDebug>
#include <QThread>
#include "lib/socketserver.h"
#include "lib/socketserveralive.h"
#include "lib/nfcthread.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

void myMessageHandler(QtMsgType type, const QMessageLogContext &context,const QString &msg)
{
    QString txt = QDateTime::currentDateTime().toString("dd-MM-yy hh:mm:ss")+" ";
    switch (type) {
    case QtDebugMsg:
        txt += "Debug: " + msg;
        break;
    case QtWarningMsg:
        txt += "Warning: " + msg;
    break;
    case QtCriticalMsg:
        txt += "Critical: " + msg;
    break;
    case QtFatalMsg:
        txt += "Fatal: " + msg;
        abort();
    }
    QFile outFile;
    if(QString(getenv("USER"))=="alberto"){

        outFile.setFileName("confdis.log");

    }else{
        outFile.setFileName("/var/log/confdis.log");
    }
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

void sighandler(int signum)
{
    qDebug() << "Receive signal number: " << signum;
    qApp->exit(0);

}

int main(int argc, char *argv[])
{

    //setenv("QT_QPA_PLATFORM","linuxfb",1);

    signal(SIGQUIT, sighandler);
    signal(SIGHUP, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    QApplication a(argc, argv);


    //p.waitForFinished();
    qInstallMessageHandler(myMessageHandler);

    SocketServer server;
    SocketServerAlive serverAlive;
    //if(QString(getenv("USER"))!="alberto"){
        NfcThread nfcTh;
        nfcTh.start();
    //}


    MainWindow w;

    w.show();

    return a.exec();
}
