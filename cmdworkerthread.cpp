#include "cmdworkerthread.h"

CmdWorkerThread::CmdWorkerThread(quint32 n, commandFn fn, const QByteArray &data)
{
    _n = n;
    _fn = fn;
    _data = data;
}
