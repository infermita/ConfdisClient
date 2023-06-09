#ifndef LEDTHREAD_H
#define LEDTHREAD_H

#include <QObject>
#include <QThread>

class LedThread : public QThread
{
    Q_OBJECT
public:
    LedThread();
    int max,set;
private:
    void run();
    int pin,dl;

};

#endif // LEDTHREAD_H
