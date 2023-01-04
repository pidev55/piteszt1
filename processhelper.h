#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include <QString>

class ProcessHelper
{
    struct Output
    {
    public:
        QString stdOut;
        QString stdErr;
        int exitCode;
        QString ToString();
    };
public:
    static const QString SEPARATOR;

    //static QString Execute(const QString& cmd);
    static QString Execute(const QStringList &cmds);
    static Output Execute(const QString &cmd);
};

#endif // PROCESSHELPER_H
