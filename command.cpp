#include "common/logger/log.h"
#include "command.h"
#include "cmdworkerthread.h"

QList<Command::Cmd> Command::_commands = QList<Command::Cmd>();

//Command::Command()
//{

//}

int Command::doWork(quint32 n, const QByteArray &data, QByteArray* out)
{
    Q_UNUSED(out)

    Commands cmd = static_cast<Commands>(data[0]);//data[0];
    auto command = find(cmd);
    if(!command) return -1;

    CmdWorkerThread *workerThread = new CmdWorkerThread(n, command->fn, data);
    QObject::connect(workerThread, &CmdWorkerThread::resultReady, &Command::handleResults);
    QObject::connect(workerThread, &CmdWorkerThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();

    //command->fn(data, out);

    //zInfo(QStringLiteral("command not found: %1").arg(QChar(cmd)));
    return 1;
}

void Command::handleResults(const QByteArray &s){
    if(s.isEmpty()) return;
    zInfo("handleResults: "+QString::number(s.count())+" bytes");
}


void Command::addAction(Commands cmd,  commandFn fn)
{
    if(fn == nullptr) return;

    //Commands c = static_cast<Commands>(cmd);

    bool isContains = contains(cmd);

    if(isContains)
    {
        zInfo(QStringLiteral("command already added: %1").arg(cmd));
    }
    else
    {
        _commands.append({cmd, fn});
    }
}

bool Command::contains(Commands cmd)
{
    return find(cmd);
}

Command::Cmd* Command::find(Command::Commands cmd)
{
    for(auto&i:_commands) if(i.command==cmd) return &(i);
    return nullptr;
}




