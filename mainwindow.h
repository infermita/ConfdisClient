#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "lib/ledthread.h"
#include <QUdpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static MainWindow* getInstance();
    void SetImage(QString id,bool check);
    QString GetText();
    void Exit();
    QString path;
    int idMonitor,pinOk,pinKo;
    QString idCard;
    LedThread *lT;
    QString sessionid;
    void SendImage(QString idImage,QString cardKey);
    QString nfc_status;
    void SetCall(QString status);


private:
    Ui::MainWindow *ui;
    static MainWindow* instance;
    int numCheckserver,numCheckNtp;
    QUdpSocket *udpSocket;
    int timerId;

protected:
    void timerEvent(QTimerEvent *event);


signals:
    void SigStartThCheckImage(QString id,QString cardKey,QString img);
public slots:
    void StartThGetId();
    void StartThCheckImage(QString id,QString cardKey,QString img);
    void StartThSetDefImage();
    void Ntpdate();
    void HideIp();

};

#endif // MAINWINDOW_H
