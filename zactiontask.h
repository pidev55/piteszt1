#ifndef MYTASK_H
#define MYTASK_H

#include <QObject>
#include <QRunnable>
#include "actionresult.h"

// A signal/slot használata miatt kell származtatni a QObjectből
// a task miatt pedig a QRunnable-ből is

class zActionTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    zActionTask();
    void setActionFn(const actionFn& a, const QUrlQuery& param, const QByteArray& c, qintptr desc);
signals:
    // a fő szálat értesíti és adja át az eredményt
    void Result(ActionResult e, qintptr desc);

protected:
    void run();
    actionFn aFn;
    QUrlQuery aParam;
    QByteArray content;
    qintptr desc;
};

#endif // MYTASK_H
