#include "aliveworker.h"
#include "doworkcmd.h"
#include "mounthelper.h"
#include "processhelper.h"
#include "recordhelper.h"
#include "testhelper.h"
#include <sys/sysinfo.h>
#include <QDateTime>
#include <QDebug>
#include "common/helper/textfilehelper/textfilehelper.h"
#include "common/logger/log.h"
#include "httpthreadedserver.h"
#include "settings.h"
#include <QApplication>


extern HTTPThreadedServer *server;
extern Settings settings;

int RecordHelper::_rec_frames;
RecordWorker RecordHelper::_recordWorker;
QDir RecordHelper::dir;
bool RecordHelper::rec;
QDateTime RecordHelper::start_time_stamp;
QVarLengthArray<PressureData> RecordHelper::pressuredata;
int RecordHelper::datacount;
int RecordHelper::_fps = 0;
bool RecordHelper::_isInited = true;
AliveWorker* RecordHelper::_aliveWorker=nullptr;

PressureHelperEvent events; //a kép event miatt kell példány

RecordHelper::RecordHelper()
{
    QObject::connect(&events, &PressureHelperEvent::newframe, &RecordHelper::getPressures);
}

RecordHelper::~RecordHelper(){
//    delete pressuredata; pressuredata = nullptr;
}

void RecordHelper::init(AliveWorker* aliveWorker, bool _rec)
{
    _isInited = false;
    rec = _rec;
    start_time_stamp = QDateTime::currentDateTimeUtc();
    _aliveWorker = aliveWorker;
    _isInited = true;
}

void RecordHelper::Rec(const QString &values, bool isSysinfo)
{
    //if(!pressuredata) return;
    auto a = PressureData{
            QDateTime::currentDateTimeUtc(),
            values
            };

    pressuredata.append(a);

    //pressuredata.insert(datacount, a);

    datacount++;

    if(datacount>=_rec_frames){
        StopRec();
        //UninitRec();
    }

    if(isSysinfo)
    {
        int fps = 1000/_recordWorker.interval();
        if(datacount % fps)
        {
            int free_Mbytes, total_Mbytes;

            if(getSysInfo(&total_Mbytes, &free_Mbytes))
            {
                auto stringem = QString("%3 frame: Free/TotalRAM: %1/%2 MB\n").arg(free_Mbytes).arg(total_Mbytes).arg(datacount);
                emit events.message(stringem);
            }
        }
    }
}

bool RecordHelper::getSysInfo(int* total_Mbytes, int *free_Mbytes)
{
    struct sysinfo info;

    if(!sysinfo(&info))
    {
        *free_Mbytes = (info.freeram*info.mem_unit)/(1024*1024);       //MB
        *total_Mbytes = (info.totalram*info.mem_unit)/(1024*1024);       //MB

        return true;
   }
   *free_Mbytes = *total_Mbytes = 0;
   return false;
}

int RecordHelper::getDatacount()
{
    return datacount;
}

void RecordHelper::getPressures(const QString& values)
{
    if(rec)
    {
        Rec(values);
    }
}

//RecordHelper::StartRecState RecordHelper::StartWorker(int fps, bool isApiMode)
//{
//    if(!isinit) return false;
//    auto isok = worker.start(1000/fps, isApiMode);
//    return isok;
//}


RecordHelper::StartRecState::State RecordHelper::StartWorker(int fps)
{
    if(!PressureHelper::isInited()) return StartRecState::NotInited;
    if(_recordWorker.isActive()) return StartRecState::Active; //ha már el van startolva, nem lehet átstartolni

    //worker.init(&PressureHelper::getAll2);

//    _rec_frames = fps*sec;
//    pressuredata = new QVarLengthArray<PressureData>(_rec_frames);

//    datacount = 0;
//    pressuredata->clear();

    auto isok = _recordWorker.start(1000/fps);
    if(!isok) return StartRecState::NotStarted;

    return StartRecState::Ok;
}

bool RecordHelper::StopWorker(){
    zInfo("StopCameraWorker..");
    if(!_isInited) return false;
    if(!_recordWorker.isActive())
        return false;// elindult
    _recordWorker.stop();
    zInfo("StopCameraWorker ok");
    return true;
}

bool RecordHelper::UnRec()
{
    zInfo("UnRec..");
    if(settings.issendalive() && _aliveWorker && !_aliveWorker->isActive()){
        _aliveWorker->start();
    }
    if(settings.isstopserver() && server && server->isPaused){
        QMetaObject::invokeMethod(server, "resume");
    }
    rec = false;
    zInfo("UnRec ok");
    return true;
}



//RecordHelper::UninitRecState RecordHelper::UninitRec()
//{
//    if(!worker.isActive()) return UninitRecState::NotActive;

//    datacount = 0;
//    pressuredata->clear();

//    return UninitRecState::Ok;
//}
RecordHelper::StartRecState RecordHelper::StartRec(bool isSync, int fps, int sec, const QString &pressuresfolder, QString *str)
{
    static bool lock = false;
    if(lock) return StartRecState::Locked;
    lock = true;
    auto a = StartRec2(isSync, fps, sec, pressuresfolder, str);
    lock = false;
    return a;
}

RecordHelper::StartRecState RecordHelper::StartRec2(bool isSync, int fps, int sec, const QString &pressuresfolder, QString *str)
{
    zInfo("StartRec..");
    if(!PressureHelper::isInited()) return StartRecState::NotInited;
    if(_recordWorker.isActive()) return StartRecState::Active; //ha már el van startolva, nem lehet átstartolni
    if(rec) return StartRecState::Rec;    
    if(!MountHelper::isMounted(MountHelper::MountPathTypes::Images))
        return StartRecState::NotMounted;

    zInfo("StartRec init..");
    _recordWorker.init(&PressureHelper::getAll2);

    _rec_frames = fps*sec;
    _fps = fps;

    rec = true;

    pressuredata.resize(_rec_frames);
    pressuredata.clear();
    datacount = 0;    

    auto fn =MountHelper::imagesPath(pressuresfolder);

    dir.setPath(fn);
    if (!dir.exists()) QDir().mkpath(dir.path());
    if(str) *str=pressuresfolder;

    zInfo("StartRec init ok");
    if(!isSync)
    {
        if(!_recordWorker.isActive()){
            zInfo("StartRec starting worker..");
            auto state = StartWorker(fps);
            if(state != StartRecState::Ok) return StartRecState::NotStarted;
            zInfo("StartRec worker ok");
        }
    }

    zInfo("StartRec waiting..");
    if(settings.isstopalive() && _aliveWorker && _aliveWorker->isActive()){
        _aliveWorker->stop();
    }

    if(settings.isstopserver() && server && !server->isPaused){
        QMetaObject::invokeMethod(server, "pause");
        auto pr = server->pending_requests.count();
        zInfo("pending_requests: "+QString::number(pr));
    }

    QApplication::processEvents();
    _recordWorker.processEvents();

    zInfo("StartRec ok");
    emit events.started();    
    return StartRecState::Ok;
}


RecordHelper::StopRecState RecordHelper::StopRec(){
    zInfo("StopRec..")
    static bool stopping = false;
    if(stopping) return StopRecState::Locked;
    stopping = true;
    auto a = StopRec2();

    if(settings.issendalive() && _aliveWorker && !_aliveWorker->isActive()){
        _aliveWorker->start();
    }
    if(settings.isstopserver() && server){
        QMetaObject::invokeMethod(server, "resume");
    }

    doWorkCmd::RecStartSyncEnable();

    stopping = false;
    zInfo("StopRec stopped")
    return a;
}

RecordHelper::StopRecState RecordHelper::StopRec2()
{


    if(!rec) return StopRecState::NotRec;
    auto t1 = QDateTime::currentDateTimeUtc();
    if(!_recordWorker.interval()) return StopRecState::NotRec;
    int fps =1000/_recordWorker.interval();
    if(_recordWorker.isActive()) _recordWorker.stop();

    qDebug()<<QString("Recording is over, %1 data fragments have been captured. Writing to disk...").arg(datacount);
    QFile data_file;
    auto t2 = QDateTime::currentDateTimeUtc();
    QString fullname_csv = dir.filePath("data.txt");
    QString ps;
    for(int index = 0; index < datacount; index++)
    {
        auto x = pressuredata.at(index);
        auto str = QStringLiteral("%1;%2\n").arg(x.timeStamp.toString(Qt::DateFormat::ISODateWithMs)).arg(x.pressures);
        ps.append(str);

        if(!(index % fps)) emit events.message("saved: "+QString::number(index));
    }
    com::helper::TextFileHelper::save(ps, fullname_csv);

    auto t3 = QDateTime::currentDateTimeUtc();
    qint64 msecs = t2.msecsTo(t3);
    emit events.message(QStringLiteral("saved %1 pics - %2 msecs").arg(datacount).arg(msecs));
    emit events.stopped();

    QStringList l;
    l.append("StartRec:"+start_time_stamp.toString(Qt::DateFormat::ISODateWithMs));
    l.append("StopRec:"+t1.toString(Qt::DateFormat::ISODateWithMs));
    l.append("StartSave:"+t2.toString(Qt::DateFormat::ISODateWithMs));
    l.append("Saved:"+t3.toString(Qt::DateFormat::ISODateWithMs));
    auto t4 = QDateTime::currentDateTimeUtc();
    l.append("Finished:"+t4.toString(Qt::DateFormat::ISODateWithMs));
    fullname_csv = dir.filePath("stats.txt");
    com::helper::TextFileHelper::save(l.join("\n"), fullname_csv);

    rec = false;
    datacount = 0;
    pressuredata.clear();
    return StopRecState::Ok;
}
