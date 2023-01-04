#include"common/logger/log.h"
#include "doworkcmd.h"
#include "recordhelper.h"
#include <QThread>
#include "settings.h"
#include <sys/time.h>

extern Settings settings;

doWorkCmd::TestR doWorkCmd::_testr = TestR();
doWorkCmd::RecStartSyncR doWorkCmd::_recStartSyncR = RecStartSyncR();


int doWorkCmd::TestCmd0(quint32 n, const QByteArray &data, QByteArray *out)
{
    Q_UNUSED(data)
    Q_UNUSED(out)

    //auto level = Errlevels::toString(Errlevels::TRACE);
    //auto msg = Log::logToString(Errlevels::TRACE, "a", "b", "c");
    zTrace();
    _testr.test0r++;
    zInfo(QString::number(n)+" test: "+QString::number(_testr.test0r))
    //while(true){}
    return 0;
}

int doWorkCmd::TestCmd1(quint32 n, const QByteArray &data, QByteArray *out)
{
    Q_UNUSED(n)
    Q_UNUSED(data)
    Q_UNUSED(out)

    _testr.test1r++;
    return 0;
}

int doWorkCmd::TestCmd2(quint32 n, const QByteArray &data, QByteArray *out)
{
    Q_UNUSED(n)
    Q_UNUSED(data)
    Q_UNUSED(out)

    _testr.test2r++;
    return 0;
}

// amikor várunk vissza adatot
int doWorkCmd::GetTestR(quint32 n, const QByteArray &data, QByteArray* out)
{
    Q_UNUSED(n)
    Q_UNUSED(data)

    if(!out) return -1;
    out->clear();
    out->append((char*)&_testr, sizeof(TestR));
    return 0;
}

/*
struct StTest
{
    int iNum1;
    char chNum;
    int iNum2;
};
QByteArray arrTest;
StTest test;
arrTest.append((char*)&test, sizeof(StTest));

***
***
***bool isRecStarted = false;
        bool isRecStopping = false;

StTest *pTest = (StTest*)arrTest.data();

*/

// TODO 99 itt elindítjuk, a recet starting státuszba rakjuk és időzítővel indítjuk
// ha már indítva volt újra időzítjük
// a getrecstatus fogja megmondani, mi a rák van
// ha nem indul egy kamera, mindent le kell állítani a kliensről és hiba
// egyébként indulnak a felvételek és mindenki boldog


// TODO 99a a kamera log porton visszaüzenhetne, hogy vette az udp-s commandot
// ezt a kliensen fogyelve ki lehet találni, hogy esetleg valaki nem kapta meg??? honnan tudjuk, hogy megkapta csak még nem válaszolt?
// meg kell kérdeznünk a rest apin, hogy megkapta-e, megkapáskor egy státuszt be kell forgatni
//

void doWorkCmd::RecStartSyncEnable(){
    _recStartSyncR.isRecStarted = false;
    _recStartSyncR.isRecStarting = false;
    // _recStartSyncR.isRecStopped = false;
    _recStartSyncR.isRecStopping = false;
}

int doWorkCmd::RecStartSync(quint32 n, const QByteArray &data, QByteArray *out)
{
    Q_UNUSED(data)
    Q_UNUSED(out)

    if(_recStartSyncR.isRecStarting ||
        _recStartSyncR.isRecStarted) {
        zInfo(QString::number(n)+" SyncStarting locked");
        return -1;
    }
    zInfo(QString::number(n)+" SyncStarting..");
    _recStartSyncR.isRecStarting = true;

    int fps = RecordHelper::fps();

    static const int datal = sizeof(qint64);
    if(data.length()<=datal) return -1;

    if(!RecordHelper::isRec())  { zInfo("isRec=1"); return -1;}
    if(RecordHelper::isActive()) { zInfo("worker.isActive=1");return -1; };
    qint64 timeout = *(reinterpret_cast<const qint64*>(data.data()+1));
    if(timeout>10000) timeout = 10000;
    zInfo("SyncStarting:"+QString::number(timeout));

    QThread::msleep(timeout);

    if(_recStartSyncR.isRecStopping){
        zInfo("RecStartSync  stopping");
        _recStartSyncR.isRecStopping = false; // felhasználódott
        _recStartSyncR.isRecStarting = false;
    } else {
        auto st = RecordHelper::StartWorker(fps);
        if(st == RecordHelper::StartRecState::State::Ok){
            _recStartSyncR.isRecStarted = true;
            _recStartSyncR.isRecStarting = false;
            //zInfo("rec start OK")
        } //else {
            //_recStartSyncR.isRecStarted = false;
            //zInfo("rec star Error: "+QString::number(st))
        //}

    }
    zInfo("SyncStarting ok")
    //_recStartSyncR.isRecStarting = false;
    //lock = false;


    //lock = false;
    return
        _recStartSyncR.isRecStarted
        ?RecordHelper::StartRecState::Ok
        :RecordHelper::StartRecState::NotStarted;
}

int doWorkCmd::StopRec(quint32 n, const QByteArray &data, QByteArray *out)
{
    Q_UNUSED(data)
    Q_UNUSED(out)

    static bool tlock = false;

    static qint64 last = 0;
    qint64 most = QDateTime::currentMSecsSinceEpoch();
    auto e = most-last;
    last = most;
    if(e>settings.ptime()){
        zInfo("StopRecCmd tenabled: " +QString::number(e));
        tlock = false; // idővel kilockolhatjuk
    }
    if(tlock){
        zInfo(QString::number(n)+" StopRecCmd tlocked");
        return -1;
    }
    tlock=true;
    if(_recStartSyncR.isRecStopping) {
        zInfo(QString::number(n)+" StopRecCmd locked");
        return -1;
    }
    _recStartSyncR.isRecStopping = true;
    tlock=true;
    zInfo(QString::number(n)+" StopRecCmd..");

    if(!(_recStartSyncR.isRecStarting ||
          _recStartSyncR.isRecStarted))
    {
        _recStartSyncR.isRecStopping = false; // töröljük, mert hat a következő körre
        zInfo("nothing to stop");
        RecordHelper::UnRec(); // visszaindítjuk a szervert
        return -1;
    }

    if(_recStartSyncR.isRecStarted){
        RecordHelper::StopWorker();
    } //else {
        RecordHelper::UnRec();
    //}

    _recStartSyncR.isRecStarting = false;
    _recStartSyncR.isRecStarted = false;

    zInfo("StopRec ok")
    return 0;
}

int doWorkCmd::SetClock(quint32 n, const QByteArray &data, QByteArray *out)
{
    Q_UNUSED(n)
    Q_UNUSED(data)
    Q_UNUSED(out)

    static const int datal = sizeof(qint64);
    if(data.length()<=datal) return -1;
    static bool lock = false;
    if(lock) return 0;

    lock = true;

    qint64 ms = *(reinterpret_cast<const qint64*>(data.data()+1));
    //QThread::msleep(t);
    //auto t2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

//    timespec ts;
    struct timeval tv{
        static_cast<__time_t>(ms / 1000),
        static_cast<__suseconds_t>((ms % 1000) * 1000)};

    //tv.tv_sec = ms / 1000;
    //tv.tv_usec = (ms % 1000) * 1000;


    //auto err = clock_settime(CLOCK_REALTIME, &ts);
    auto err = settimeofday(&tv, nullptr);
    auto err2 = errno; // CAP_SYS_TIME
    //EPERM .. 1
    //int st = CameraHelper::StartRecState::NotStarted;
    //auto isok = CameraHelper::StartCameraWorker(CameraHelper::fps(), true);
    //if(isok) st = CameraHelper::StartRecState::Ok;

    zInfo("SetClock: "+QString::number(err)+':'+QString::number(err2))
    lock = false;
    return 0; // nem volt hiba
}
