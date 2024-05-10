#include "nfcthread.h"
#include <QDebug>
#include "mainwindow.h"

NfcThread::NfcThread()
{

}
void NfcThread::run(){

    int i,sl = 1;
    QString id;
    const nfc_modulation nmMifare = {
        .nmt = NMT_ISO14443A,
        .nbr = NBR_106,
      };
    sleep(2);
    while(1){

        MainWindow::getInstance()->nfc_status = "0";

        nfc_init(&context);
        if (context == NULL) {
            qDebug() << "Unable to init libnfc (malloc)";
            sl = 60;

        }else{

            //qDebug() << "Context inizializzato";

            pnd = nfc_open(context, NULL);
            if (pnd == NULL) {
                qDebug() << "ERROR: %s. Unable to open NFC device.";
                sl = 60;

            }else{

                sl = 1;

                qDebug() << "Nfc aperto";

                if (nfc_initiator_init(pnd) < 0) {
                    qDebug() << "nfc_initiator_init";
                    nfc_close(pnd);
                    nfc_exit(context);

                }else{

                    MainWindow::getInstance()->nfc_status = "1";

                    qDebug() << "Nfc iniator";
                    if(nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &nt) > 0){

                        qDebug() << "Nfc letto";
                        //ui->label->setText("ALBERTO MARI ER CAPO DELLA GECO :)");
                        /*
                        char *s;
                        str_nfc_target(&s, &nt, true);
                        qDebug() << "Informazioni: " << QString(s);
                        nfc_free(s);
                        */
                        //uint8_t abtRx[1024];
                        id = "";
                        for(i = 0; i < nt.nti.nai.szUidLen;i++){

                            //id += QString::number(nt.nti.nai.abtUid[i],16);
                            QString hex;
                            id += hex.sprintf("%02x",nt.nti.nai.abtUid[i]).toUpper();

                        }
                        MainWindow::getInstance()->SetImage(id,true);

                        while(!nfc_initiator_target_is_present(pnd,&nt)){
                            sleep(1);
                        }
                        nfc_close(pnd);
                        nfc_exit(context);
                        //QTimer::singleShot(100,this,SLOT(NfcPresent()));


                    }

                }

            }
        }

        nfc_exit(context);

        sleep(sl);

    }

}
