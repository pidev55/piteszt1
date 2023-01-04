#include "shutdowner.h"
#include <QCoreApplication>
//#include <QProcess>
#include <QString>
#include "common/logger/log.h"
#include <QDBusInterface>
#include <QDebug>
#include <QDBusPendingReply>
//#include <QDBusPendingReply>

//#include "common/helper/ProcessHelper/processhelper.h"

/*
Also possible there is a need to add a file /etc/polkit-1/localauthority/50-local.d/10-enable-shutdown.pkla to suppress interactive authentication requirement:

[Enable shoutdown for users]
Identity=unix-group:users
Action=org.freedesktop.login1;org.freedesktop.login1.power-off;org.freedesktop.login1.power-off-ignore-inhibit;org.freedesktop.login1.power-off-multiple-sessions
ResultAny=yes
ResultInactive=yes
ResultActive=yes
*/
Shutdowner::Shutdowner()
{

}


//void Shutdowner::Shutdown()
//{
//    QString cmd = QStringLiteral("/home/pi/pizero_poweroff/bin/pizero_poweroff");
//    //com::helper::ProcessHelper::Execute(cmd);
//    //qint64 pid;
//    QProcess process;
//    static QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
//    env.insert("LD_LIBRARY_PATH", "/usr/lib"); // workaround - https://bugreports.qt.io/browse/QTBUG-2284
//    process.setProcessEnvironment(env);
//    static auto path = QCoreApplication::applicationDirPath();
//    process.setWorkingDirectory(path);

//    process.start(cmd);

//    process.waitForFinished(-1); // will wait forever until finished
//}

int Shutdowner::Shutdown()
{

    QDBusInterface logind{
        QStringLiteral("org.freedesktop.login1"),
        QStringLiteral("/org/freedesktop/login1"),
        QStringLiteral("org.freedesktop.login1.Manager"),
        QDBusConnection::systemBus()
    };

    const auto message = logind.callWithArgumentList(QDBus::Block, QStringLiteral("CanPowerOff"), {});
    QDBusPendingReply<QString> canPowerOff = message;

    Q_ASSERT(canPowerOff.isFinished());
    if (canPowerOff.isError())
    {
        const auto error = canPowerOff.error();
        qWarning().noquote() << QDBusInterface::tr("Asynchronous call finished with error: %1 (%2)").arg(error.name(), error.message());
        return EXIT_FAILURE;
    }
    if (canPowerOff.value() == "yes")
    {
        QDBusPendingReply<> powerOff = logind.callWithArgumentList(QDBus::Block, QStringLiteral("PowerOff"), {true, });
        Q_ASSERT(powerOff.isFinished());
        if (powerOff.isError())
        {
            const auto error = powerOff.error();
            qWarning().noquote() << QDBusInterface::tr("Asynchronous call finished with error: %1 (%2)").arg(error.name(), error.message());
            return EXIT_FAILURE;
        }
    }
    else
    {
        qCritical().noquote()
                << QCoreApplication::translate("poweroff", "Can't power off: CanPowerOff() result is %1")
                   .arg(canPowerOff.value());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

