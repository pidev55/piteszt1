#ifndef RECORDWORKER_H
#define RECORDWORKER_H

#include <QTimer>
#include <QThread>

using workFn = bool(*)();

class RecordWorker : public QObject
{
    Q_OBJECT
public:
    QTimer *timer = nullptr;
    RecordWorker();
    ~RecordWorker();

    bool init(workFn);
    bool start(int);
    bool stop();
    bool isActive(){return (isinit&&timer)?timer->isActive():false;}
    int interval(){return (isinit&&timer)?timer->interval():0;}
    void processEvents();
private:
    static workFn wf1;
    bool isinit;

    Q_SIGNAL void start_signal(int i);
    Q_SIGNAL void stop_signal();
private slots:
    void start_slot(int);
    void stop_slot();
};

#endif // RECORDWORKER_H
