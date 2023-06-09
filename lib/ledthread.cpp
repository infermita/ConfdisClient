#include "ledthread.h"
#include <wiringPi.h>
#include "mainwindow.h"
LedThread::LedThread()
{

    //pin=MainWindow::getInstance()->pin;

}

void LedThread::run(){

    int i;

    pin = MainWindow::getInstance()->pinOk;

    dl = 500;
    if(max>4){
        dl = 150;
        pin = MainWindow::getInstance()->pinKo;
    }

    if(max==4){

        for(i=0; i < max;i++){

            digitalWrite (MainWindow::getInstance()->pinOk, HIGH) ; delay (dl) ;
            digitalWrite (MainWindow::getInstance()->pinOk,  LOW);
            digitalWrite (MainWindow::getInstance()->pinKo,  HIGH) ; delay (dl) ;
            digitalWrite (MainWindow::getInstance()->pinKo,  LOW);

        }

    }else{
        for(i=0; i < max;i++){

            digitalWrite (pin, HIGH) ; delay (dl) ;
            digitalWrite (pin,  LOW) ; delay (dl) ;

        }
    }

}
