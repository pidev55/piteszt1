#include "processhelper.h"
#include <QProcess>

#ifdef Q_OS_LINUX
 #define NEWLINE "\n"
#elif defined(Q_OS_WIN)
 #define NEWLINE "\r\n"
#endif

const QString ProcessHelper::SEPARATOR = NEWLINE+QStringLiteral("stderr")+NEWLINE;


QString ProcessHelper::Output::ToString(){
    QString e;

    if(!stdOut.isEmpty())
    {
        if(!e.isEmpty()) e+=SEPARATOR;
        e+=stdOut;
    }
    if(!stdErr.isEmpty())
    {
        if(!e.isEmpty()) e+=SEPARATOR;
        e+=stdErr;
    }
    if(!e.isEmpty()) e+=SEPARATOR;
    e+=QStringLiteral("exitCode: %1").arg(exitCode);
    return e;
}

ProcessHelper::Output ProcessHelper::Execute(const QString& cmd){
    QProcess process;
    process.start(cmd);
    process.waitForFinished(-1); // will wait forever until finished
    return {process.readAllStandardOutput(), process.readAllStandardError(), process.exitCode()};
}

QString ProcessHelper::Execute(const QStringList& cmds){
    Output e;

    foreach(auto cmd, cmds){
        auto r = Execute(cmd);
        e.stdOut += r.stdOut;
        e.stdErr += r.stdErr;
        e.exitCode = r.exitCode;
    }
    return e.ToString();
}
