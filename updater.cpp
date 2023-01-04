#include "common/logger/log.h"
#include "updater.h"
#include "mounthelper.h"
#include "testhelper.h"
#include <QApplication>
#include <QDateTime>
#include <QDirIterator>
#include "unistd.h"
#include "ztcpsocket.h"
#include <sys/statvfs.h>
#include "settings.h"
#include "restarter.h"
#include "common/helper/textfilehelper/textfilehelper.h"
#include "userhelper.h"

extern Settings settings;
extern Restarter* restarter;

//const QString Updater::_updaterFn=QStringLiteral("upizero_mcp3008_api.sh");
//const QString Updater::_projectName = (QT_TARGET);
//const QString Updater::_commonlib=QStringLiteral("commonlib");
//#ifdef QT_DEBUG
//    const QString Updater::_projectCommonLib=QStringLiteral("commonlib_debug_armv6zk");
//#else
//    const QString Updater::_projectCommonLib=QStringLiteral("commonlib_release_armv6zk");
//#endif


const QString Updater::DeviceName = QStringLiteral("insole");

Updater::Updater()
{

}

bool Updater::StartUpdate(const Updater::Version& ver)
{
    this->_ver = ver;
    this->start();
    return true;
}

void Updater::run(){
    _isUpdating= true;
    if(!_ver.isValid()){ zInfo("invalid version"); return;}
    if(!restarter){ zInfo("no restarter"); return;}

    if(Update4(_ver)){
        restarter->restart();
    } else {
        zInfo("update unsuccesful");
        _isUpdating = false;
    }
}

//bool Updater::UpdateFile(const QString& target, const QString& dst, const QString& zip_path){
//    if(QFile(target).exists()){ zInfo("file up to date: "+ target); return false; } //már létezik}

//    if(!QFile::copy(dst, target)) return false;//{"cannotcopy",ActionResult::Status::Codes::OK};
//    QString cmd = QStringLiteral(R"(unzip -xo %1 -d %2)").arg(target).arg(zip_path);
//    auto out = ProcessHelper::Execute(cmd);
//    return true;
//}



//bool Updater::isCopy(const QFileInfo& fi,
//                     const QString& appdir,
//                     QString *target,
//                     const QString& label){
//    if(!target) return false;
//    if(fi.isFile()) {
//        if(!label.isEmpty()) zInfo(label+": " + fi.fileName());
//        *target = QDir(appdir).filePath(fi.fileName());
//        return !QFile(*target).exists();
//    }
//    if(!label.isEmpty()) zInfo(label+": no file");
//    return false;
//}

bool Updater::Copy(const QFileInfo& fi,
                   const QString& target,
                   bool isExecutable,
                   const QString& label){
    auto pp =
        QFileDevice::Permission::ReadOwner |
        QFileDevice::Permission::WriteOwner |
        QFileDevice::Permission::ReadGroup |
        QFileDevice::Permission::WriteGroup |
        QFileDevice::Permission::ReadOther
        ;

    if(isExecutable) {
        pp |= QFileDevice::Permission::ExeOwner |
            QFileDevice::Permission::ExeGroup;
    }

    zInfo(label+" copy...");
    if(!QFile::copy(fi.filePath(), target)) return false;
    zInfo("copy ok");
    if(!QFile(target).setPermissions(pp)) return false;
    zInfo("setPermissions ok");
    return true;
}

bool Updater::SetUserGrp(const QString& path){
    int gid,uid;
    bool uisok = UserHelper::GetUserId(settings.user(), &uid);
    bool gisok = UserHelper::GetGroupId(settings.group(), &gid);
    if(gisok && uisok){
        QFileInfo fi(path);
        chown(fi.absoluteFilePath().toStdString().c_str(), uid,gid);
        zInfo("setOwnerAndGroup ok");
        return true;
        }
    return false;
    }


qint64 Updater::GetFreeSpace2(){
    struct statvfs buf;
    if (statvfs("/", &buf) == -1) return -1;
    auto a =  (buf.f_bavail * buf.f_frsize);
    return a;
}

QString Updater::UpdaterFN(){
    auto appname = QString(QT_TARGET);
    auto updaterFn = "z"+appname+".sh";
    return updaterFn;
}

bool Updater::UploadUpdater(){
    bool ismounted = MountHelper::isMounted(MountHelper::HWUpdates);
    auto host = zTcpSocket::host();
    if(!ismounted) MountHelper::Mount(host, MountHelper::HWUpdates);
    const QString& path = MountHelper::localPath(MountHelper::HWUpdates);

    QString msg = "";

    auto updaterFn = UpdaterFN();
    msg+="updaterFn: "+updaterFn+"\n";

    auto updaterPath = QDir(path).filePath(updaterFn);
    QFileInfo updaterInf(updaterPath);

    msg+="updaterPath: "+updaterPath+"\n";
    bool isok = false;
    if(updaterInf.isFile()){
        auto appdir = qApp->applicationDirPath();
        auto target = QDir(appdir).filePath(updaterInf.fileName());
        msg+="target: "+target+"\n";

        if(QFile::exists(target)) QFile::remove(target);
        isok = Copy(updaterInf, target, true, "updater");
        msg+=QString("copy: ")+(isok?"ok":"error")+'\n';
        isok = SetUserGrp(target);
        msg+=QString("setusrgrp: ")+(isok?"ok":"error")+'\n';
    }

    auto appdir = qApp->applicationDirPath();
    msg+="appdir: "+appdir+"\n";
    com::helper::TextFileHelper::save(msg, "/home/pi/upd_log.txt");
    return isok;
}


// appdir:
//  - éppenfutó
//  - run symlinktarget - biztos, hogy ez a legfrissebb???
    // libdir:
//  - lib symlinktarget
//  - összes .zip
//    QString h1 = QDir::home().absolutePath();
//    QString h2 = QDir("~").absolutePath();
//    QString h3 = QDir::homePath();


auto Updater::RemoveOldFiles(const QString& appdir, const QString& updateFolderName) -> bool
{
    zInfo(QStringLiteral("removing old files.."));
    static const QString REMOVED = QStringLiteral(" removed");

    bool istest = !TestHelper::remove_old_files;

    auto appname = QString(QT_TARGET);
    //auto appdir = qApp->applicationDirPath();
    QDirIterator it_appdir(appdir,
                           QDir::Filter::NoDotAndDotDot |
                           QDir::Filter::Files,
                           QDirIterator::IteratorFlag::NoIteratorFlags);
    QString homepath = QDir().absolutePath();
    auto run_fullname = QDir(homepath).filePath(TestHelper::run_symlinkname);

    QFileInfo run_fi(run_fullname);
    QString run_target;
    if(run_fi.isSymLink()){
        run_target = run_fi.symLinkTarget();
    }
    QString running_target = qApp->applicationFilePath();

    QDirIterator it_libdir(TestHelper::commonfolder,
                           QDir::Filter::NoDotAndDotDot|
                               QDir::Filter::Files |
                               QDir::Filter::Dirs,
                           QDirIterator::IteratorFlag::NoIteratorFlags);
    auto lib_fullname = QDir(TestHelper::commonfolder).filePath(TestHelper::lib_symlinkname);
    QFileInfo lib_fi(lib_fullname);
    QString lib_target;
    if(lib_fi.isSymLink()){
        lib_target = lib_fi.symLinkTarget();
    }

    while (it_appdir.hasNext())
    {
        QFileInfo fi(it_appdir.next());
        if(!run_target.isEmpty() && fi.absoluteFilePath()==run_target) continue;
        if(fi.absoluteFilePath()==running_target) continue;
        if(!fi.fileName().startsWith(appname)) continue;

        auto msg = QStringLiteral("%1").arg(fi.absoluteFilePath());
        if(!istest && QFile::remove(fi.absoluteFilePath())){
            msg+=REMOVED;
        }
        zInfo(msg);
    }

    while (it_libdir.hasNext())
    {
        QFileInfo fi(it_libdir.next());
        if(!lib_fullname.isEmpty() && fi.absoluteFilePath()==lib_fullname) continue;
        if(!lib_target.isEmpty() && fi.absoluteFilePath()==lib_target) continue;
        if(!fi.fileName().startsWith("commonlib")) continue;

        auto msg = QStringLiteral("%1").arg(fi.absoluteFilePath());
        if(!istest){
            if(fi.isFile()){
                if(QFile::remove(fi.absoluteFilePath())){
                    msg += REMOVED;
                }
            } else if(fi.isDir()){
                QDir dir(fi.absoluteFilePath());
                if(dir.removeRecursively()){
                    msg += REMOVED;
                }
            }
        } //if istest
        zInfo(msg);
    }
    //updates
    QDirIterator it_updir(updateFolderName,
                           QDir::Filter::NoDotAndDotDot |
                           QDir::Filter::Files|
                           QDir::Filter::Dirs,
                           QDirIterator::IteratorFlag::NoIteratorFlags);
    auto upd_fullname = QDir(TestHelper::commonfolder).filePath(run_target+"_ver");
    QFileInfo upd_fi(upd_fullname);
    QString upd_target;
    if(upd_fi.isSymLink()){
        upd_target = upd_fi.symLinkTarget();
    }
    zInfo("upd_target: "+upd_target);
    while (it_updir.hasNext())
    {
        QFileInfo fi(it_updir.next());

        if(!upd_fullname.isEmpty() && fi.absoluteFilePath()==upd_target) continue;
        if(!upd_fullname.isEmpty() && fi.absoluteFilePath()==upd_target+".zip") continue;

        auto msg = fi.absoluteFilePath();
        msg+= QStringLiteral(" removing.. ");
        if(!istest){
            if(fi.isFile()){
                if(QFile::remove(fi.absoluteFilePath())){
                    msg += REMOVED;
                }
            } else if(fi.isDir()){
                QDir dir(fi.absoluteFilePath());
                if(dir.removeRecursively()){
                    msg += REMOVED;
                }
            }
        } //if istest
        zInfo(msg);
    }
    return true;
}


Updater::Output Updater::Execute2(const QString& cmd){
    auto fn = qApp->applicationDirPath();

    QProcess process;
    process.setWorkingDirectory(fn);
    process.start(cmd);
    process.waitForFinished(-1); // will wait forever until finished
    Updater::Output a {process.readAllStandardOutput(),
                      process.readAllStandardError(),
                      process.exitCode()};
    return a;
}

auto Updater::Output::ToString() -> QString
{
    QString e;
    static const QString SEPARATOR = QStringLiteral("\n\n");

    if(!stdOut.isEmpty())
    {
        if(!e.isEmpty()) { e+=SEPARATOR;
        }
        e+="stdout: "+stdOut;
    }
    if(!stdErr.isEmpty())
    {
        if(!e.isEmpty()) e+=SEPARATOR;
        e+="stderr: "+stdErr;
    }
    if(!e.isEmpty()) e+=SEPARATOR;
    e+=QStringLiteral("exitCode: %1").arg(exitCode);
    return e;
}

bool Updater::Update4(Updater::Version ver){
    QString msg="update4";
    zInfo("Update4")
    //bool isUpdate =false;
    bool ismounted = MountHelper::isMounted(MountHelper::HWUpdates);
    auto host = zTcpSocket::host();
    if(!ismounted) MountHelper::Mount(host, MountHelper::HWUpdates);
    auto path = MountHelper::localPath(MountHelper::HWUpdates);
    auto static const commonpath = TestHelper::commonfolder;
    auto appname = QString(QT_TARGET);
    auto appdir = qApp->applicationDirPath();

    auto tstamp = QDateTime(QDate(1980,1,1), QTime());
    auto tstamp_cd = tstamp;
    auto tstamp_cr = tstamp;

    auto ver_str = ver.ToString();
    auto upd_name = appname+'_'+ver_str;
    auto zupd_name = upd_name+".zip";
    auto prog_fn = QDir(path).filePath(zupd_name);
    auto most_recent_prog = QFileInfo(prog_fn);

    zInfo("user: "+settings.user());
    zInfo("group: "+settings.group());

    QString cameraTarget, updateFolderName;

    // fájl-e egyáltalán, és kell-e másolni, vgy már fent van
    // ha nincs mit másolni, akkor is false, csak akkor a target is üres
    // ha nem kell másolni, akkor lehet hogy már ott van, amúgy, ekkor a target nem üres
    bool isCopyCamera = isCopy4(most_recent_prog, appdir, &cameraTarget, &updateFolderName, DeviceName);

    if(cameraTarget.isEmpty()) {
        zInfo("nincs fájl: "+prog_fn)
        return false;
    }

    // azt hogy mennyi hely kell a lmezen, azután tudjuk megmondani, hogy megvizsgáljuk a most recent méretét
    qint64 sizeNeeded = isCopyCamera?most_recent_prog.size()*3:0; // másolandó byteok összege
    qint64 sizeAvailable = GetFreeSpace2();

    zInfo("size: "+QString::number(sizeAvailable)+"/"+QString::number(sizeNeeded));
    bool diskFull = isDiskFull(sizeNeeded);

    if(diskFull){
        msg+="disk full - removing files...";
        zInfo("disk full1");
        RemoveOldFiles(appdir, updateFolderName);
        zInfo("size: "+QString::number(sizeAvailable)+"/"+QString::number(sizeNeeded));
        diskFull = isDiskFull(sizeNeeded);
        if(diskFull){
            msg+="failed\n";
            zInfo("disk full2");
        }
        else{
            msg+="ok\n";
        }
    }

    bool isok = false;
    if(!diskFull){
        if(isCopyCamera){
            msg+="cameraTarget: "+cameraTarget+"\n";
            auto isok = Copy(most_recent_prog, cameraTarget, false, "zipfile");
            msg+=QStringLiteral("copy: ")+(isok?"ok":"err")+"\n";
            if(!isok) zInfo("cannot copy: "+most_recent_prog.filePath());
            isok = SetUserGrp(cameraTarget);
            if(!isok) zInfo("cannot setusrgrp: "+most_recent_prog.filePath());
            msg+=QStringLiteral("setusrgrp: ")+(isok?"ok":"err")+"\n";
        }

        zInfo(QStringLiteral("syncing...."));
        sync();

        zInfo(QStringLiteral("updating...."));
        QString cmd = "z"+appname+".sh install "+zupd_name;

        msg+="cmd: "+cmd+"\n";
        zInfo("cmd: "+cmd)

            // todo _backup_last_to_disk -> ha besül a mutatvány, kívülről helyre lehet hozni
            // ha van ilyen fájl, akkor az besült mutatványt jelent és automatice helyreteszi

        Updater::Output out = Updater::Execute2(cmd);
        if(!out.stdOut.isEmpty()){
            msg+="out: "+out.ToString()+"\n";
            zInfo(out.ToString())
        }
        isok = out.exitCode==0;
    }
    msg+=QStringLiteral("update4: ")+((isok)?"ok":"err")+"\n";
    com::helper::TextFileHelper::save(msg, "/home/pi/upd_log.txt", true);
    return isok;
}

auto Updater::isDiskFull(qint64 sizeNeeded)->bool
{
    qint64 sizeAvailable = GetFreeSpace2();
    return ((sizeNeeded>0 && sizeAvailable<sizeNeeded) ||
            (sizeNeeded ==0 && sizeAvailable<90*1024*1024));
}

//void Updater::IsUpdate4(const QString& prog, bool *isProg, const QString& _updaterFn)
//{
//    QString cmd = QStringLiteral("%1 availables").arg(_updaterFn);
//    auto out = Execute2(cmd);
//    if(!out.stdOut.isEmpty()){
//        auto fields = out.stdOut.split(',', QString::SplitBehavior::SkipEmptyParts);
//        if(isProg && !prog.isEmpty()) *isProg = !fields.contains(prog);
//    }
//}

bool Updater::isCopy4(const QFileInfo& fi,
                      const QString& appdirFn,
                      QString *target,
                      QString *updateFolderName,
                      const QString& label){
    if(!target) return false;
    if(fi.isFile()) {
        if(!label.isEmpty()) zInfo(label+": " + fi.fileName());
        auto updateParent = QDir(appdirFn);
        updateParent.cdUp();
        *updateFolderName = updateParent.filePath("updates");
        QFileInfo d2(*updateFolderName);

        if(!d2.exists()){
            updateParent.mkpath(*updateFolderName);
            QDir du(*updateFolderName);
            QString a  =du.absolutePath();
            SetUserGrp(a);
        } else {
            if(!d2.isDir()){ // van már ilyen fájl, el kell takarítani
                //átnevez és csinál egy könyvtárat
                QFile::rename(*updateFolderName, *updateFolderName+"2");
                updateParent.mkpath(*updateFolderName);
                QDir du(*updateFolderName);
                QString a  =du.absolutePath();
                SetUserGrp(a);
            }
        }

        *target = QDir(*updateFolderName).filePath(fi.fileName());

        return !QFile(*target).exists();
    }
    if(!label.isEmpty()) zInfo(label+": no file: "+fi.fileName());
    return false;
}
