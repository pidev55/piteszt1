#include "restarter.h"
#include "common/logger/log.h"
#include <QApplication>
#include <QAbstractEventDispatcher>
//#include <QThread>
#include <QProcess>
#include <QDir>

Restarter::Restarter()
{
    QObject::connect(this, &Restarter::restartSignal, this, &Restarter::restart2);
    _isRestart = false;
}

void Restarter::restart2(Mode m)
{
    qApp->deleteLater();
//    do
//    {
//        qApp->processEvents();
//    }while(
//           QCoreApplication::has
//           //qApp->eventDispatcher()->hasPendingEvents()
//           );
    _isRestart= true;
    qApp->exit(m);
//    do
//    {
//        qApp->processEvents();
//    }while(qApp->eventDispatcher()->hasPendingEvents());
}

//int Restarter::returnValue()
//{
//   return _isRestart?Mode::Restart:Mode::Shutdown;
//}
