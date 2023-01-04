#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <QString>

class TestHelper
{
public:
    static const QString buildnumber;
    static const int alivePort = 12312;
    static const int hostPort = 8080;

    static const QString aliveAddr;
    static const QString sharedpath;
    static const QString shared_pressurespath;
    //static const QString update;

    static const int minsec = 3; //10 sec magától
    static const int maxsec = 10; //10 sec magától

    static const int fps = 60;
    static const QString symlink;
    static const QString run_symlinkname;
    static const QString lib_symlinkname;
    static const QString commonfolder;

    static const int maxThreads = 25;

    static const bool remove_old_files = true; // false nem töröl csak úgy tesz
};

#endif // TESTHELPER_H
