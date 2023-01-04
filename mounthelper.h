#ifndef MOUNTHELPER_H
#define MOUNTHELPER_H

#include "common/macrofactory/macro.h"
#include <QMap>
#include <QString>

class MountHelper
{
public:
    enum MountPathTypes:int{
        Images=1, HWUpdates
    };

    struct MountPath{
        QString remoteName;
        QString localPath;
    };

    static const QMap<MountPathTypes, MountPath> _mountPaths;

    struct MountState{
    public:
        enum State:int{Ok=0,ArgumentError,MountError};
        MountState(State s){_state = s;}//_state_desc=QString();}
        MountState(State s, const QString &d):MountState(s) {_state_desc=d;}
        bool isOk(){return !_state;}
        bool isErr(){return _state;}
    private:
        State _state;
        QString _state_desc;
    public:
        QString ToString(){
            static const QString _Ok = nameof(Ok);
            static const QString _ArgumentError = nameof(ArgumentError);
            static const QString _MountError = nameof(MountError);
            switch(_state){
            case Ok: return _Ok;
            case ArgumentError: return _ArgumentError;
            case MountError: return _MountError;
            default: return "unknown";
            }
        };

    };
    //static MountState Mount_old(const QString&p);
    static MountState Mount(const QString& host, MountPathTypes mtype);
    static bool isMounted(MountPathTypes mtype, QString* = nullptr);
    static QString imagesPath(const QString& fp);
    static QString localPath(MountPathTypes mtype);
    //static bool isMounted_old(, QString* = nullptr);
};

#endif // MOUNTHELPER_H
