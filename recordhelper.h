#ifndef RECORDHELPER_H
#define RECORDHELPER_H

#include <QDateTime>
#include <QDir>
#include <QString>
#include "common/macrofactory/macro.h"
#include "recordworker.h"
#include "pressurehelper.h"
#include "pressurehelperevent.h"
#include "globals.h"
#include "aliveworker.h"

struct PressureData{
    QDateTime timeStamp;
    QString pressures;
};

class RecordHelper
{
public:
    RecordHelper();
    ~RecordHelper();
    static bool isRec(){return rec;}
    static void init(AliveWorker* aliveWorker, bool);
    static bool isActive(){return _recordWorker.isActive();}
    static bool interval(){return _recordWorker.interval();}

    /*start*/
//    struct InitRecState{
//    public:
//        enum State:int{Ok=0,NotInited,Active,NotStarted};
//        InitRecState(State s){_state = s;}
//        bool isOk(){return !_state;}
//        bool isErr(){return _state;}
//    private:
//        State _state;
//    public:
//        QString ToString(){
//            static const QString _Ok = nameof(Ok);
//            static const QString _NotInited = nameof(NotInited);
//            static const QString _Active = nameof(Active);
//            static const QString _NotStarted = nameof(NotStarted);
//            switch(_state){
//                case Ok: return _Ok;
//                case NotInited: return _NotInited;
//                case Active: return _Active;
//                case NotStarted: return _NotStarted;
//                default: return "unknown";
//            }
//        }
//    };


    /*stop camera*/
    struct UninitRecState{
    public:
        enum State:int{Ok=0,NotInited,NotActive, NotOpened};
        UninitRecState(State s){_state = s;}
        bool isOk(){return !_state;}
        bool isErr(){return _state;}
    private:
        State _state;
    public:
        QString ToString(){
            static const QString _Ok = nameof(Ok);
            static const QString _NotInited = nameof(NotInited);
            static const QString _NotActive = nameof(NotActive);
            static const QString _NotOpened = nameof(NotOpened);
            switch(_state){
                case Ok: return _Ok;
                case NotInited: return _NotInited;
                case NotActive: return _NotActive;
                case NotOpened: return _NotOpened;
                default: return "unknown";
            }
        };
    };
    static UninitRecState UninitRec();

    /*start rec*/
    struct StartRecState{
    public:
        enum State:int{Ok=0,NotInited,Active,Rec,NotStarted,NotActive,NotMounted, Locked};
        StartRecState(State s){_state = s;}
        bool isOk(){return !_state;}
        bool isErr(){return _state;}
    private:
        State _state;
    public:
        QString ToString(){
            static const QString _Ok = nameof(Ok);
            static const QString _Rec = nameof(Rec);
            static const QString _Active = nameof(Active);
            static const QString _NotInited = nameof(NotInited);
            static const QString _NotStarted = nameof(NotStarted);
            static const QString _NotActive = nameof(NotActive);
            static const QString _NotMounted = nameof(NotMounted);
            static const QString _Locked = nameof(Locked);

            switch(_state){
                case Ok: return _Ok;
                case Rec: return _Rec;
                case Active: return _Active;
                case NotInited: return _NotInited;
                case NotStarted: return _NotStarted;
                case NotActive: return _NotActive;
                case NotMounted: return _NotMounted;
                case Locked: return _Locked;

                default: return "unknown";
            }
        }
    };
    static StartRecState StartRec(bool isSync, int fps, int sec, const QString& videofolder, QString* str = nullptr);
    static StartRecState StartRec2(bool isSync, int fps, int sec, const QString& videofolder, QString* str = nullptr);

    /*stop rec*/
    struct StopRecState{
    public:
        enum State:int{Ok=0,NotRec,Locked};
        StopRecState(State s){_state = s;}
        bool isOk(){return !_state;}
        bool isErr(){return _state;}
    private:
        State _state;
    public:
        QString ToString(){
            static const QString _Ok = nameof(Ok);
            static const QString _NotRec = nameof(NotRec);
            static const QString _Locked = nameof(Locked);
            switch(_state){
                case Ok: return _Ok;
                case NotRec: return _NotRec;
                case Locked: return _Locked;
                default: return "unknown";
            }
        };
    };
    static StopRecState StopRec();
    static StopRecState StopRec2();

    static int getDatacount();
    static StartRecState::State StartWorker(int fps);
    static bool StopWorker();
    static int fps(){return _fps;}

    static bool UnRec();
private:
    static int _rec_frames;
    static RecordWorker _recordWorker;
    static QDir dir;
    static bool rec;
    static bool _isInited;
    static QDateTime start_time_stamp;
    static QVarLengthArray<PressureData> pressuredata;
    static void Rec(const QString&, bool isSysinfo = false);
    static bool getSysInfo(int* , int *);
    static int datacount;
    static int _fps;
    static AliveWorker* _aliveWorker;
private slots:
    static void getPressures(const QString&);
};

#endif // RECORDHELPER_H
