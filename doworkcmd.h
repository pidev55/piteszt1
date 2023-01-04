#ifndef DOWORKCMD_H
#define DOWORKCMD_H

#include <QByteArray>



class doWorkCmd
{
public:    

    struct TestR{
        int test0r=-1;
        int test1r=-1;
        int test2r=-1;
    };

    static TestR _testr;
    static int TestCmd0(quint32 n, const QByteArray &data, QByteArray *out);
    static int TestCmd1(quint32 n, const QByteArray &data, QByteArray *out);
    static int TestCmd2(quint32 n, const QByteArray &data, QByteArray *out);

    struct RecStartSyncR
    {
        bool isRecStarting = false;
        bool isRecStarted = false;
        bool isRecStopping = false;
    };

    static RecStartSyncR _recStartSyncR;

    static int RecStartSync(quint32 n, const QByteArray &data, QByteArray *out);
    static int StopRec(quint32 n, const QByteArray &data, QByteArray *out);

    static int GetTestR(quint32 n, const QByteArray &data, QByteArray *out);
    static int SetClock(quint32 n, const QByteArray &data, QByteArray *out);
    static void RecStartSyncEnable();
};

#endif // DOWORKCMD_H
