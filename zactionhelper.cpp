#include "globals.h"
#include "zactionhelper.h"

zActionHelper::zActionHelper()
{

}

//const zAction* zActionHelper::find(const QList<zAction>& actions, QString m, QString p)
//{
//    for (auto i = actions.begin(); i != actions.end(); ++i)
//    //zforeach(i, actions)
//    {
//        if(i->method == m && i->path == p)
//        {
//            return i;
//            //auto e = i->operator->());
//            //return e;
//        }
//    }
//    return nullptr;
//}


const Action* zActionHelper::find(const QList<Action>& actions, QString m, QString p)
{
    //for (auto i = actions.begin(); i != actions.end(); ++i)
    for(auto&i:actions)
    {
        if(i.method == m && i.path == p)
        {
            return &i;
        }
    }
    return nullptr;
}

const Action *zActionHelper::find(const QList<Action>& actions, const Request& r)
{
    return find(actions, r.method, r.url.path());
}

bool zActionHelper::contains(const QList<Action>& actions, QString m, QString p)
{
    return(find(actions, m, p)!=nullptr);
}
