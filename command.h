#ifndef COMMAND_H
#define COMMAND_H

#include <QByteArray>
#include <QList>
//#include "actionresult.h"


typedef int (*commandFn)(quint32 n, const QByteArray &data, QByteArray*);

class Command
{
public:    
//    Command();

    enum Commands:uchar{
        GetTestR = '@',
        Test = 'A',
        RecStartSync,
        StopRec,
        SetClock
    };

    static int doWork(quint32 n, const QByteArray& data, QByteArray* out);
    static void addAction(Commands m, commandFn fn);
    static void handleResults(const QByteArray &s);
private:
    struct Cmd
    {
        Commands command;
        commandFn fn;
    };

    static QList<Cmd> _commands;

    static bool contains(Commands cmd);
    static Cmd* find(Commands cmd);
};

#endif // COMMAND_H
