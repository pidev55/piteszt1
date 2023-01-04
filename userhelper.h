#ifndef USERHELPER_H
#define USERHELPER_H

#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <QString>


class UserHelper
{
public:
    static QString GetUserName()
    {
        uid_t uid = geteuid();
        struct passwd *pw = getpwuid (uid);
        if (pw)
        {
            return QString(pw->pw_name);
        }
        return {};
    }


    static bool GetUserId(const QString& uname, int* uid)
    {
        struct passwd *pw = getpwnam(uname.toStdString().c_str());
        if (pw)
        {
            if(uid) *uid=pw->pw_uid;
            return true;
        }
        return false;
    }

    static bool GetGroupId(const QString& gname, int* gid)
    {
        struct group *g = getgrnam(gname.toStdString().c_str());
        if (g)
        {
            if(gid) *gid=g->gr_gid;
            return true;
        }
        return false;
    }
};

#endif // USERHELPER_H
