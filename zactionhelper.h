#ifndef ACTIONHELPER_H
#define ACTIONHELPER_H

#include "actionresult.h"
#include "request.h"

class zActionHelper
{
public:
    zActionHelper();

    static const Action* find(const QList<Action>& actions, QString r, QString p);
    static const Action* find(const QList<Action>& actions, const Request& r);
    static bool contains(const QList<Action>& actions, QString r, QString p);
};

#endif // ACTIONHELPER_H
