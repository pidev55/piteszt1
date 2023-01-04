#include "mounthelper.h"
#include "processhelper.h"
#include <QStringList>

#include <QDir>

const QMap<MountHelper::MountPathTypes, MountHelper::MountPath>
    MountHelper::_mountPaths {
        {Images, {"examimages", "/mnt/images"}},
        {HWUpdates, {"hwupdatefiles", "/mnt/shared"}}
    };


bool MountHelper::isMounted(MountPathTypes mtype, QString* s){
    auto p = _mountPaths.value(mtype);
    //if(p.isEmpty()) return false;

    //TODO előtte ping - ha a host nem megy nem elérhető
    QString cmd = QStringLiteral(R"(df --output=target %1)").arg(p.localPath);
    auto out = ProcessHelper::Execute(cmd);

    if(out.exitCode!=0) return false;
    if(out.stdOut.isEmpty()) return false;
    auto a = out.stdOut.split('\n');
    if(a.size()<2) return false;
    if(a[1].isEmpty()) return false;

    if(s) *s=a[1];
    return a[1]!="/";
}

QString MountHelper::imagesPath(const QString &fp)
{
    return QDir(_mountPaths.value(MountPathTypes::Images).localPath).filePath(fp);
}

QString MountHelper::localPath(MountPathTypes mtype)
{
    return _mountPaths.value(mtype).localPath;
}


MountHelper::MountState MountHelper::Mount(const QString& host,
                                           MountPathTypes mtype){
    Q_UNUSED(host)
    auto p = _mountPaths.value(mtype);
    QString cmd =
        QStringLiteral(R"(sudo mount -t cifs //%1/%2 %3 -o username=pi,password=kercerece55,domain=WORKGROUP,gid=users,uid=500,rw,dir_mode=0777,file_mode=0666)").arg(host).arg(p.remoteName).arg(p.localPath);
        //    QStringLiteral(R"(mount %1)").arg(p.localPath);
    auto out = ProcessHelper::Execute(cmd);
    if(!out.exitCode) return MountState::Ok;
    if(out.stdErr.isEmpty()) return MountState::Ok;

    return {MountState::MountError,out.ToString()};
}
