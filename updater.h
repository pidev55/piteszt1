#ifndef UPDATER_H
#define UPDATER_H
#include <QFileInfo>
#include <QString>
#include <QThread>

//#include "restarter.h"

//extern Restarter *restarter;

class Updater : public QThread
{
    Q_OBJECT
public:
    Updater();

    struct Version{
        enum ReleaseType {None,Alpha,Beta,ReleaseCandidate};
        int major=-1;
        int minor=-1;
        ReleaseType releaseType=None;
        int patch=-1;
        int build=-1;

        QString ToStringRT(ReleaseType t){
            switch(t){
            case Alpha: return "a";
            case Beta: return "b";
            case ReleaseCandidate: return "rc";
            default: return "";
            }
        };

        static ReleaseType ParseRT(QString str){
            if(str.startsWith('a')) return ReleaseType::Alpha;
            if(str.startsWith('b')) return ReleaseType::Beta;
            if(str.startsWith("rc")) return ReleaseType::ReleaseCandidate;
            return ReleaseType::None;
        };

        static Version Parse(QString str){
            Version v;
            auto fs=str.split('.');
            if(fs.length()>=1) v.major = fs[0].toInt();
            if(fs.length()>=2){
                int ix = fs[1].indexOf('-');
                if(ix>0){
                    auto p0 = fs[1].left(ix);
                    v.minor = p0.toInt();
                    auto p1 = fs[1].mid(ix+1);
                    v.releaseType = ParseRT(p1);
                } else {
                    v.minor = fs[1].toInt();
                }
            }
            if(fs.length()>=3){
                int ix = fs[2].indexOf('-');
                if(ix>0){
                    auto p0 = fs[2].left(ix);
                    v.patch = p0.toInt();
                    auto p1 = fs[2].mid(ix+1);
                    v.releaseType = ParseRT(p1);
                } else {
                    v.patch = fs[2].toInt();
                }
            }
            if(fs.length()>=4) v.build = fs[3].toInt();
            return v;
        };

        QString ToString(){
            QString v = QString::number(major)+'.'+QString::number(minor);
            if(patch>-1) v+='.'+QString::number(patch);
            QString t = ToStringRT(releaseType);
            if(!t.isEmpty()) v+='-'+t;
            if(build>-1)v+='.'+QString::number(build);
            return v;
        };

        bool isValid(){
          return this->major>=0 && this->minor>=0;
        };
    };


    bool StartUpdate(const Updater::Version& ver);
    bool isUpdating(){
        return _isUpdating;
    }
//    static bool isCopy(const QFileInfo &fi,
//                       const QString &appdir,
//                       QString *target,
//                       const QString& label = QString());

    static bool isCopy4(const QFileInfo &fi,
                        const QString &appdir,
                        QString *target,
                        QString *updateFolderName,
                        const QString& label = QString());

    static bool Copy(const QFileInfo &fi,
                     const QString &target,
                     bool isExecutable,
                     const QString &label);

    //static qint64 GetFreeSpace();
    static qint64 GetFreeSpace2();
    //enum ValidateR:int { Error = -1, Invalid, Valid};
    //static ValidateR ValidateVer(const QString &ver);

    //static void IsUpdate(const QString &lib, const QString &prog, bool *isLib, bool *isProg);
    //static void IsUpdate4(const QString &prog, bool *isProg, const QString& _updaterFn);

    static bool UploadUpdater();
    static bool RemoveOldFiles(const QString& appdir, const QString& updateFolderName);
    static bool isDiskFull(qint64 sizeNeeded);
    static QString UpdaterFN();
    static bool SetUserGrp(const QString &fi);
private:
    static const QString DeviceName;
    bool _isUpdating=false;
    Updater::Version _ver;
    void run() override;
    bool static Update4(Updater::Version ver);
//    bool static UpdateFile(const QString& target,
//                           const QString& dst,
//                           const QString& zip_path);

    struct Output
    {
    public:
        QString stdOut;
        QString stdErr;
        int exitCode;
        QString ToString();
    };

    static Output Execute2(const QString& cmd);
};

#endif // UPDATER_H
