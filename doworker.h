#ifndef DOWORKER_H
#define DOWORKER_H

#include <QThread>
#include <QUrlQuery>
#include "actionworker.h"


class doWorker : public QThread
{
    Q_OBJECT

public:
    ActionWorker _fn;
    QUrlQuery _param;
    QByteArray _content;
    ActionResult _result;

    doWorker(const QUrlQuery &param, const QByteArray &content, ActionWorker fn)
    {
        _fn=fn;
        _param = QUrlQuery(param);
        _content = QByteArray(content);
    };

private:
    void run(){ if(_fn) _result = _fn(_param, _content); }
};

#endif // DOWORKER_H
