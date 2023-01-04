#ifndef ALIVEWORKER_H
#define ALIVEWORKER_H
#include "ztcpsocket.h"

#include <QThread>
#include <QTimer>

using aliveFn = void(*)(void*);

class AliveWorker : public QObject
{
    Q_OBJECT
public:
    QTimer *_timer = nullptr;
    AliveWorker();
    ~AliveWorker();
    QThread _thread;

    bool init(QObject*, aliveFn, int i);
    bool start();
    bool stop();
    bool isActive(){return (isinit&&_timer)?_timer->isActive():false;}
    int interval(){return (isinit&&_timer)?_timer->interval():0;}
    void processEvents();
private:
    QObject* _object;
    aliveFn _fn;
    bool isinit;
    int _interval=1000;

signals:
    void start_signal();
    void stop_signal();
private slots:
    void start_slot();
    void stop_slot();
    void on_timeout();
};

#endif // ALIVEWORKER_H
