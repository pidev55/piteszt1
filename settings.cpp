#include "settings.h"
#include "common/helper/textfilehelper/textfilehelper.h"


#include <QDir>
#include <QSet>
#include <QMap>
#include <QApplication>

const QString Settings::SETTINGS_FILENAME = QStringLiteral("settings.ini");
const Settings::Key  Settings::key;
Settings::Data  Settings::data;

Settings::Settings()=default;

auto Settings::Load() -> bool
{
    auto fn = fileName();
    auto d = Settings::LoadIni(fn);
    if(d.isEmpty()) return false;
    int i;
    //qreal r;
    QString a;

    if(get(&i, key.threads, d)) setThreads(i);

    if(get(&a, key.aliveaddr, d)) setAliveaddr(a);
    if(get(&i, key.issendalive, d)) setIssendalive(i);
    if(get(&i, key.aliveinterval, d)) setAliveinterval(i);

    if(get(&i, key.isstopalive, d)) setIsStopalive(i);
    if(get(&i, key.isstopserver, d)) setIsStopserver(i);

    return true;
}

auto Settings::fileName() -> QString
{
//    auto f1 =  QDir::current();
//    auto f2 = QDir::currentPath();
//    auto f3 = qApp->applicationFilePath();

    auto fn = QDir(qApp->applicationDirPath()).filePath(SETTINGS_FILENAME);

    return fn;
}

auto Settings::get(int* a,
                   const QString& k,
                   const QMap<QString,QString>& d) -> bool
{
    if(!a) return false;
    if(!d.contains(k)) return false;
    bool isok;
    *a=d[k].toInt(&isok);
    return isok;
}

auto Settings::get(qreal* a,
                   const QString& k,
                   const QMap<QString,QString>& d) -> bool
{
    if(!a) return false;
    if(!d.contains(k)) return false;
    bool isok;
    *a=d[k].toDouble(&isok);
    return isok;
}

auto Settings::get(QString* a,
                   const QString& k,
                   const QMap<QString,QString>& d)->bool
{
    if(!a) return false;
    if(!d.contains(k)) return false;
    *a=d[k];
    return true;
}

auto Settings::get(bool* a,
                   const QString& k,
                   const QMap<QString,QString>& d) ->bool
{
    if(!a) return false;
    static const QSet<QString> ok{"ok","yes","true"};
    if(!d.contains(k)) return false;

    int e;
    bool isok;
    auto s = d[k];
    e=s.toInt(&isok);
    if(isok){
        *a = e;
    } else {
        *a = ok.contains(s.toLower());
    }
    return true;
}

void Settings::set(int a, const QString& k, QMap<QString,QString>& d){
    auto v = QString::number(a);
    if(d.contains(k)) d[k]=v; else d.insert(k,v);
}

void Settings::set(const QString& v, const QString& k, QMap<QString,QString>& d){
    if(d.contains(k)) d[k]=v; else d.insert(k,v);
}

void Settings::set(bool a, const QString& k, QMap<QString,QString>& d){
    QString v = a?QStringLiteral("true"):QStringLiteral("false");
    if(d.contains(k)) d[k]=v; else d.insert(k,v);
}

void Settings::set(qreal a, const QString& k, QMap<QString,QString>& d){
    auto v = QString::number(a);
    if(d.contains(k)) d[k]=v; else d.insert(k,v);
}

auto Settings::Save() -> bool
{
    QMap<QString, QString> d;

    set(threads(), key.threads, d);

    set(aliveaddr(), key.aliveaddr, d);
    set(issendalive(), key.issendalive, d);
    set(aliveinterval(), key.aliveinterval, d);

    set(isstopalive(), key.isstopalive, d);
    set(isstopserver(), key.isstopserver, d);

    auto fn = fileName();
    return Settings::SaveIni(fn, d);
}

auto Settings::LoadIni(const QString& fn) ->QMap<QString, QString>
{
    QMap<QString, QString> map;

    auto lines = com::helper::TextFileHelper::loadLines(fn);
    if(lines.isEmpty()) return {};
    for(auto&l:lines)
    {
        if(l.isEmpty()) continue;
        if(l.startsWith('#')) continue;
        if(l.length()<3) continue;
        int ix = l.indexOf('=');
        if(ix<1) continue;
        if(ix>=l.length()-1) continue;
        QString key = l.left(ix).trimmed();
        QString value = l.mid(ix+1).trimmed();
        if(key.isEmpty()) continue;
        map.insert(key, value);
    }

    return map;
}

auto Settings::SaveIni(const QString &fn,
                       const QMap<QString, QString> &d) -> bool
{
    QStringList lines;
    if(d.isEmpty()) return false;

    for(auto i = d.begin();i!=d.end();++i)
    {
        lines.append(i.key()+'='+i.value());
    }
    if(lines.isEmpty()) return true;


    bool isNew = !QFileInfo(fn).exists();

    bool isOk = com::helper::TextFileHelper::save(lines.join('\n'), fn);

    if(isNew){
        auto pp =
            QFileDevice::Permission::ReadOwner |
            QFileDevice::Permission::WriteOwner |
            QFileDevice::Permission::ReadGroup |
            QFileDevice::Permission::WriteGroup |
            QFileDevice::Permission::ReadOther |
            QFileDevice::Permission::WriteOther
            ;

        if(!QFile(fn).setPermissions(pp)) return false;
    }

    return isOk;
}
