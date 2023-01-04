#ifndef CMDWORKERTHREAD_H
#define CMDWORKERTHREAD_H

#include <QThread>
#include "command.h"


class CmdWorkerThread: public QThread
{
    Q_OBJECT
    void run() override {
        if(_fn) _fn(_n, _data, &_result);
        emit resultReady(_result);
    }
signals:
    void resultReady(const QByteArray &s);
private:
    commandFn _fn;
    QByteArray _data;
    QByteArray _result;
    quint32 _n;
public:
    CmdWorkerThread(quint32 n, commandFn fn, const QByteArray& data);
};

#endif // CMDWORKERTHREAD_H
