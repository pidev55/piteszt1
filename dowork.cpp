#include "dowork.h"
//#include "zlog.h"
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QString>
#include <QApplication>
#include <QVector>
#include <QString>
#ifdef RPI
#include <QElapsedTimer>
#include <wiringPi.h>
#endif
//#include <linux/spi/spidev.h>
#include "buildnumber.h"
#include "pressurehelper.h"
#include "testhelper.h"
#include "common/macrofactory/macro.h"
#include "common/helper/textfilehelper/textfilehelper.h"
#include "mounthelper.h"
#include "common/logger/log.h"
#include "globals.h"
#include "processhelper.h"
#include "recordhelper.h"
#include "restarter.h"
#include "updater.h"
#include "ztcpsocket.h"
#include "instance.h"

QVector<doWorker*> doWork::workers;
extern Restarter *restarter;
extern Updater *updater;
extern Instance _instance;
//extern zLog zlog;

doWork::doWork(){}

const QString doWork::ERROR = QStringLiteral("error:");
const QString doWork::STATUS = QStringLiteral("status:");

const ActionResult doWork::_ok("ok", ActionResult::Status::Codes::OK);
//const ActionResult doWork::_locked(ERROR+nameof(_locked), ActionResult::Status::Codes::ServiceUnavailable);
//const ActionResult doWork::_notimplemented(ERROR+nameof(_notimplemented), ActionResult::Status::Codes::NotImplemented);
//const ActionResult doWork::_busy(ERROR+nameof(_busy), ActionResult::Status::Codes::ServiceUnavailable);
const ActionResult doWork::_not_mounted(STATUS+nameof(_not_mounted), ActionResult::Status::Codes::OK);
const ActionResult doWork::_wrong_ix(ERROR+nameof(_wrong_ix), ActionResult::Status::Codes::BadRequest);
const ActionResult doWork::_no_ix(ERROR+nameof(_no_ix), ActionResult::Status::Codes::BadRequest);
const ActionResult doWork::_uninitialised(ERROR+nameof(_uninitialised), ActionResult::Status::Codes::NoContent);
const ActionResult doWork::_no_data(ERROR+nameof(_no_data), ActionResult::Status::Codes::BadRequest);
const ActionResult doWork::_no_file(ERROR+nameof(_no_file), ActionResult::Status::Codes::BadRequest);
//const ActionResult doWork::_wrong_data(ERROR+nameof(_wrong_data), ActionResult::Status::Codes::BadRequest);
const ActionResult doWork::_no_folder(ERROR+nameof(_no_folder), ActionResult::Status::Codes::BadRequest);
const ActionResult doWork::_name_too_long(ERROR+nameof(_name_too_long), ActionResult::Status::Codes::BadRequest);
const ActionResult doWork::_wrong_name(ERROR+nameof(_wrong_name), ActionResult::Status::Codes::BadRequest);
const ActionResult doWork::_data_too_long(ERROR+nameof(_data_too_long), ActionResult::Status::Codes::BadRequest);
const ActionResult doWork::_not_set(STATUS+nameof(_not_set), ActionResult::Status::Codes::OK);

const QString doWork::_cannot_start_rec = nameof(_cannot_start_rec);

bool doWork::time_lock=false;

const doWork::Error doWork::_error;

ActionResult doWork::work(const QUrlQuery &param, const QByteArray &content, ActionWorker fn, bool* lock, int timeout)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    if(lock){
        if(*lock)
            return _error.locked;
        *lock = true;
    }


    ActionResult r;

    if(fn)
    {
        if(timeout<0)
        {
            r = fn(param, content);
        }
        else
        {
            if(timeout<100) timeout = 100;
            QElapsedTimer elapsed_timer;
            elapsed_timer.start();
            auto w = new doWorker(param, content, fn);
            static int i=1;
            w->setObjectName("worker_"+QString::number(i++));
            w->start();

            QEventLoop loop;
            QTimer t;
            t.connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);

            while(!w->isFinished() && !elapsed_timer.hasExpired(timeout)){
                t.start(4);
                loop.exec();
            }

            if(w->isFinished())
            {
                r = w->_result;
                delete w;
            }
            else
            {
                zInfo("timeout: "+ w->objectName());
                if(lock)*lock = false;
                w->setPriority(QThread::IdlePriority);

                w->exit(1);
                workers.append(w);

                if(workers.length()>=10){
                    bool isRe = false;
                    for(auto&v:workers){
                        if(!v) continue;
                        if(v->isRunning()){
                            zInfo("terminate:" + v->objectName());
                            v->terminate();
                        }
                        if(v->isFinished()){
                            zInfo("delete:" + v->objectName());
                            isRe = true;
                            delete v;v=nullptr;
                        }
                    }
                    if(isRe){
                        workers.removeAll(nullptr);
                    }
                }

                QString msg = QStringLiteral("timeout")+'|'+QString::number(workers.length());
                //if(!CameraHelper::isGrabOk) msg+= RS+QStringLiteral("grabfailed");
                //if(!CameraHelper::isOpenOk) msg+= RS+QStringLiteral("openfailed");
                r = {msg, ActionResult::Status::Codes::RequestTimeout};
            }
        }
    }
    else r = _error.notimplemented;

    if(lock)*lock = false;

    //if(workers.count()>=TestHelper::maxThreads) restarter->restart(Restarter::Mode::Restart);
    return r;
}

//zActionResult doWork::dolgoz1(const QUrlQuery& param, const QByteArray& content)
//{
//    QJsonObject j
//    {
//        {"kulcs1", QStringLiteral("érték1")},
//        {"kulcs2", QStringLiteral("érték2")},
//        {"kulcs3", 55}
//    };

//#ifdef RPI
//    if(param.hasQueryItem("led"))
//    {
//        int ledState;
//        if(param.queryItemValue("led")=="on")
//        {
//            ledState = HIGH;//digitalWrite (1, HIGH);
//        }
//        else
//        {
//            ledState = LOW;//digitalWrite (1, LOW);
//        }
//        digitalWrite (1, ledState);
//        j.insert("LED", ledState);
//    }
//#endif

//    return zActionResult(QJsonDocument(j).toJson(QJsonDocument::Indented), zAction::contentType::JSON);
//}

//zActionResult doWork::dolgoz2(const QUrlQuery& param, const QByteArray& content)
//{
//    return zActionResult(QStringLiteral("árvíztűrőtükörfúrógép 2 :%1").arg(param.query()).toUtf8(), zAction::contentType::TEXT);
//}

//active
ActionResult doWork::active(const QUrlQuery& param, const QByteArray& content)
{
    static bool lock;
    return work(param, content, &active2, &lock, -1);
}

ActionResult doWork::active2(const QUrlQuery& param, const QByteArray& content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    static const ActionResult r(QStringLiteral("active").toUtf8());
    return r;
}

ActionResult doWork::version(const QUrlQuery& param, const QByteArray& content)
{
    static bool lock;
    return work(param, content, &version2, &lock, -1);
}

ActionResult doWork::version2(const QUrlQuery& param, const QByteArray& content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    QString d = QString(QT_TARGET);
    QString b = Buildnumber::toString();
    ActionResult r(d+':'+b);
    return r;
}


ActionResult doWork::version4(const QUrlQuery& param, const QByteArray& content)
{
    static bool lock;
    return work(param, content, &version4_2, &lock, -1);
}

ActionResult doWork::version4_2(const QUrlQuery& param, const QByteArray& content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    QString d = QStringLiteral(QT_TARGET);
    QString b = Buildnumber::toString();

    auto appdir = QCoreApplication::applicationDirPath();
    auto linkfullname = QDir(appdir).filePath(d+'_'+b+"_ver");

    QFileInfo fi(linkfullname);
    QString pver;
    if(fi.exists() && fi.isSymLink()){
        QString a = fi.symLinkTarget();
        int ix = a.lastIndexOf('_');
        if(ix>0) pver = a.mid(ix+1);
    }
    static const ActionResult r(pver.isEmpty()?QStringLiteral("unknown"):pver);

    return r;
}

ActionResult doWork::version5(const QUrlQuery& param, const QByteArray& content)
{
    static bool lock;
    return work(param, content, &version5_2, &lock, -1);
}

ActionResult doWork::version5_2(const QUrlQuery& param, const QByteArray& content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    QString homepath = QDir().absolutePath();
    QString runfillname = QDir(homepath).filePath(QStringLiteral("run"));

    QFileInfo run(runfillname);
    QString linkfullname;
    QString pver;

    if(run.exists() && run.isSymLink()){
        linkfullname = run.symLinkTarget()+"_ver";
    }
    if(!linkfullname.isEmpty())
    {
        QFileInfo fi(linkfullname);
        if(fi.exists() && fi.isSymLink()){
            QString a = fi.symLinkTarget();
            int ix = a.lastIndexOf('_');
            if(ix>0) pver = a.mid(ix+1);
        }
    }
    static const ActionResult r(pver.isEmpty()?QStringLiteral("unknown"):pver);
    return r;
}


ActionResult doWork::instance(const QUrlQuery& param, const QByteArray& content)
{
    static bool lock;
    return work(param, content, &instance2, &lock, -1);
}

ActionResult doWork::instance2(const QUrlQuery& param, const QByteArray& content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    QString d = QString(QT_TARGET);
    QString i = _instance.ToString();
    ActionResult r(d+':'+i);
    return r;
}

ActionResult doWork::hwinfo(const QUrlQuery& param, const QByteArray& content)
{
    static bool lock;
    return work(param, content, &hwinfo2, &lock, -1);
}


ActionResult doWork::hwinfo2(const QUrlQuery& param, const QByteArray& content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    //QString d = QString(QT_TARGET);
    //QString i = _instance.ToString();
    auto appdir = qApp->applicationDirPath();
    auto hwinfoFn = QDir(appdir).filePath("hwinfo.csv");
    QString hwinfo= com::helper::TextFileHelper::load(hwinfoFn);

    ActionResult r(hwinfo);
    return r;
}

ActionResult doWork::get_data(const QUrlQuery& param, const QByteArray& content)
{
    static bool lock;
    return work(param, content, &get_data2, &lock, -1);
}

ActionResult doWork::get_batt(const QUrlQuery& param, const QByteArray& content)
{
    static bool lock;
    return work(param, content, &get_batt2, &lock, -1);
}

ActionResult doWork::get_data2(const QUrlQuery& param, const QByteArray& content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    auto p = PressureHelper::getAll();
    QString r = p.toString();
    return ActionResult(r.toUtf8());
}

ActionResult doWork::get_batt2(const QUrlQuery& param, const QByteArray& content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    auto p = PressureHelper::getBatt();
    QString r = QString::number(p);
    return ActionResult(r.toUtf8());
}

ActionResult doWork::get_data_length(const QUrlQuery& param, const QByteArray& content)
{
    static bool lock;
    return work(param, content, &get_data_length2, &lock, -1);
}

ActionResult doWork::get_data_length2(const QUrlQuery& param, const QByteArray& content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    auto p = PressureValues::data_length;
    return ActionResult(QString::number(p).toUtf8());
}

ActionResult doWork::get_storage_status(const QUrlQuery &param, const QByteArray &content)
{
    static bool lock;
    return work(param, content, &getstoragestatus2, &lock, -1);
}



ActionResult doWork::get_insole_status2(const QUrlQuery &param, const QByteArray &content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    bool header;

    static const QString header_key=nameof(header);
    header = param.hasQueryItem(header_key);

    QStringList e;

    e.append(header?"isRec":QString::number(RecordHelper::isRec()));//ins
    e.append(header?"isActive":QString::number(RecordHelper::isActive()));//van worker
    e.append(header?"count":QString::number(RecordHelper::getDatacount()));//ins hanyadok kockánál tart
    e.append(header?"interval[ms]":QString::number(RecordHelper::interval()));//van

    return ActionResult(e.join(';'));
}

ActionResult doWork::set_storage_mount(const QUrlQuery &param, const QByteArray &content)
{
    static bool lock;
    return work(param, content, &mountstorage2, &lock, -1);
}

//const QString doWork::DATAPATH_KEY= "data";
//////
//ActionResult doWork::mountstorage2(const QUrlQuery &param, const QByteArray &content)
//{
//    Q_UNUSED(param)
//    Q_UNUSED(content)

//    static const QString cannot_mount = nameof(cannot_mount);


//    const QString& path = (param.hasQueryItem(DATAPATH_KEY))?TestHelper::shared_pressurespath:TestHelper::sharedpath;

//    if(MountHelper::isMounted(path)){
//        static const ActionResult r(QStringLiteral("ok\n%1").arg(path).toUtf8());
//        return r;
//    }

//    auto mount_state = MountHelper::Mount(path);
//    if(mount_state.isOk()){
//        static const ActionResult r(QStringLiteral("ok\n%1").arg(path).toUtf8());
//        return r;
//    }
//    return ActionResult((STATUS+mount_state.ToString()).toUtf8());
//}

////static const QString images_key= "images";
//ActionResult doWork::getstoragestatus2(const QUrlQuery &param, const QByteArray &content)
//{
//    Q_UNUSED(param)
//    Q_UNUSED(content)


//    const QString& path = (param.hasQueryItem(DATAPATH_KEY))?TestHelper::shared_pressurespath:TestHelper::sharedpath;

//    auto isok = MountHelper::isMounted(path);
//    if(isok){
//        static const ActionResult r(QStringLiteral("ok\n%1").arg(path).toUtf8());
//        return r;
//    }

//    static const QString _not_mounted = STATUS+nameof(_not_mounted);
//    static const ActionResult r(_not_mounted.toUtf8());
//    return r;
//}

ActionResult doWork::mountstorage2(const QUrlQuery &param, const QByteArray &content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    static const QString cannot_mount = nameof(cannot_mount);

    //if(!GuiHelper::canUseApi()) return _error.busy;

    static const QString images_key= "images";
    MountHelper::MountPathTypes mtype =
        (param.hasQueryItem(images_key))
        ?MountHelper::MountPathTypes::Images
        :MountHelper::MountPathTypes::HWUpdates;

    QString localpath;
    if(MountHelper::isMounted(mtype, &localpath)) return _ok.append(localpath);

    auto host = zTcpSocket::host();
    auto mount_state = MountHelper::Mount(host, mtype);
    if(mount_state.isOk()) return _ok.append(MountHelper::localPath(mtype));
    return ActionResult(STATUS+mount_state.ToString());
}

ActionResult doWork::getstoragestatus2(const QUrlQuery &param, const QByteArray &content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    //if(!GuiHelper::canUseApi()) return _error.busy;
    static const QString images_key= "images";
    MountHelper::MountPathTypes mtype =
        (param.hasQueryItem(images_key))
        ?MountHelper::MountPathTypes::Images
        :MountHelper::MountPathTypes::HWUpdates;

    QString localpath;
    bool isok = MountHelper::isMounted(mtype, &localpath);
    if(isok) return _ok.append(localpath);
    return _not_mounted;
}


ActionResult doWork::set_rec_start(const QUrlQuery &param, const QByteArray &content)
{
    static bool lock;
    return work(param, content, &startrec2, &lock, -1);
}

ActionResult doWork::startrec2(const QUrlQuery &param, const QByteArray &content)
{
    //Q_UNUSED(param)
    Q_UNUSED(content)

    bool sync=false;
    int sec=0;
    QString datafolder = "";
    static const QString datafolder_key = nameof(datafolder);
    static const QString sec_key = nameof(sec);
    static const QString sync_key = nameof(sync);

    if(param.hasQueryItem(datafolder_key))
    {
        datafolder = param.queryItemValue(datafolder_key);
    }

    if(datafolder.isEmpty()) return _error.wrong_data;

    if(param.hasQueryItem(sec_key))
    {
        bool isok;
        sec = param.queryItemValue(sec_key).toInt(&isok);
    }

    if(param.hasQueryItem(sync_key))
    {
        sync = true;
    }

    if(sec<=0){
        sec = TestHelper::minsec;
    }
    else if(sec>TestHelper::maxsec){
        sec = TestHelper::maxsec;
    }

    static const QString cannot_start_rec = nameof(cannot_start_rec);
    static const QString cannot_stop_rec = nameof(cannot_stop_rec);

//    auto start_state = RecordHelper::StartWorker(TestHelper::fps, sec);
//    if(start_state.isErr()) return ActionResult(_cannot_start_rec + ": "+start_state.ToString());

    QString e;
    auto rec_state = RecordHelper::StartRec(sync, TestHelper::fps, sec,datafolder, &e);
    if(rec_state.isErr())
    {
        QString err = cannot_start_rec + ": "+rec_state.ToString();
        return ActionResult(err);
    }

    return _ok.append(e);
}

ActionResult doWork::set_rec_stop(const QUrlQuery &param, const QByteArray &content)
{
    static bool lock;
    return work(param, content, &stoprec2, &lock, -1);
}

ActionResult doWork::get_insole_status(const QUrlQuery &param, const QByteArray &content)
{
    static bool lock;
    return work(param, content, &get_insole_status2, &lock, -1);
}

ActionResult doWork::stoprec2(const QUrlQuery &param, const QByteArray &content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    static const QString cannot_stop_rec = nameof(cannot_stop_rec);
    static const QString cannot_stop_camera = nameof(cannot_stop_camera);

    auto stoprec_state = RecordHelper::StopRec();
    if(stoprec_state.isErr()) return ActionResult(cannot_stop_rec + ": "+stoprec_state.ToString());

    // TODO rec leáll, kamera leáll, kamera bezár.
//    auto stop_state = RecordHelper::UninitRec();
//    if(stop_state.isErr()) return ActionResult(cannot_stop_camera + ": "+stop_state.ToString());

    return _ok;
}

ActionResult doWork::update4(const QUrlQuery &param, const QByteArray &content)
{
    static bool lock;
    return work(param, content, &update4_2, &lock, -1);
}

ActionResult doWork::update4_2(const QUrlQuery &param, const QByteArray &content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    QString ver;
    Updater::Version version;
    static const QString ver_key = nameof(ver);

    //if(!GuiHelper::canUseApi()) return _error.busy;

    if(param.hasQueryItem(ver_key)){
        ver = param.queryItemValue(ver_key);
        version = Updater::Version::Parse(ver);
    }


    bool isUploader = Updater::UploadUpdater();
    if(!isUploader){
        return _error.wrong_data.append("cannot upload");
    }

//    Updater::ValidateR isok = Updater::ValidateVer(ver);

//    if(isok==Updater::ValidateR::Error) return _error.wrong_data.append("validator unavailable");
//    if(isok==Updater::ValidateR::Invalid) return _error.wrong_data.append(ver_key);

    updater->StartUpdate(version);
    return _ok;
}

ActionResult doWork::restart(const QUrlQuery &param, const QByteArray &content)
{
    static bool lock;
    return work(param, content, &restart2, &lock, -1);
}

ActionResult doWork::restart2(const QUrlQuery &param, const QByteArray &content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    restarter->restart();
    return _ok;
}


ActionResult doWork::shutdown(const QUrlQuery &param, const QByteArray &content)
{
    static bool lock;
    return work(param, content, &shutdown2, &lock, -1);
}

ActionResult doWork::shutdown2(const QUrlQuery &param, const QByteArray &content)
{
    Q_UNUSED(param)
    Q_UNUSED(content)

    restarter->shutdown();
    return _ok;
}

ActionResult doWork::set_time_ntp(const QUrlQuery &param, const QByteArray &content)
{
    return work(param, content, &set_time_ntp2, &time_lock, -1);
}

ActionResult doWork::set_time_ntp2(const QUrlQuery &param, const QByteArray &content)
{
    Q_UNUSED(content)
    Q_UNUSED(param)

    QString cmd = QStringLiteral(R"(sudo ntpdate cpu)");
    auto out = ProcessHelper::Execute(cmd);
    if(!out.exitCode) return _ok.append(out.stdOut);
    if(out.stdErr.isEmpty()) return _ok;

    return _not_set;
}
