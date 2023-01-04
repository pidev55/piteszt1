#ifndef SETTINGS_H
#define SETTINGS_H

#include <QList>
#include <QString>
#include "common/macrofactory/macro.h"

class Settings
{
private:
    static const QString SETTINGS_FILENAME;

    struct Key{
        const QString issendalive = nameof(Data::issendalive);
        const QString aliveinterval = nameof(Data::aliveinterval);
        const QString aliveaddr = nameof(Data::aliveaddr);
        const QString threads = nameof(Data::threads);
        const QString ptime = nameof(Data::ptime);
        const QString isstopalive = nameof(Data::isstopalive);
        const QString isstopserver = nameof(Data::isstopserver);
    };

    struct Data{
        int threads = 3;

        QString aliveaddr="cpu";
        bool issendalive=true;
        int aliveinterval=1000;

        unsigned long ptime = 100;

        bool isstopalive=false;
        bool isstopserver=true;

        QString user = "pi";
        QString group = "users";
    };

    static const Key key;
    static Data data;

public:
    Settings();

    bool Load();
    bool Save();

    static QString fileName();

    bool validatePercent(int i){ return i>=0&&i<=100;}
    bool validatePtime(unsigned long i){ return i>=10&&i<=3000;}
    bool validateString(const QString& s){
        if(s.isEmpty()) return false;
        if(s.length()>20) return false;
        return true;
    }
    bool validateThreads(int i){ return i>=1&&i<=10;}
    bool validatAliveinterval(int i){ return i>=100&&i<=5000;}

    bool validateContrast(int i){return validatePercent(i);}
    bool validateBrightness(int i){return validatePercent(i);}
    bool validateSaturation(int i){return validatePercent(i);}
    bool validateIso(int i){ return i>=0&&i<=800;}

    bool setPtime(unsigned long i, bool* changed = nullptr){
        if(!validatePtime(i)) return false;
        if(changed) *changed = data.ptime!=i;
        data.ptime=i;
        return true;
    }

    bool setThreads(int i, bool* changed = nullptr){
        if(!validateThreads(i)) return false;
        if(changed) *changed = data.threads!=i;
        data.threads=i;
        return true;
    }

    bool setAliveaddr(QString a, bool* changed = nullptr){
        if(!validateString(a)) return false;
        if(changed) *changed = data.aliveaddr!=a;
        data.aliveaddr=a;
        return true;
    }

    bool setIssendalive(bool i, bool* changed = nullptr){
        if(changed) *changed = data.issendalive!=i;
        data.issendalive=i;
        return true;
    }

//    bool setRemoveOldFiles(bool i, bool* changed = nullptr){
//        if(changed) *changed = data.remove_old_files!=i;
//        data.remove_old_files=i;
//        return true;
//    }

    bool setAliveinterval(int i, bool* changed = nullptr){
        if(!validatAliveinterval(i)) return false;
        if(changed) *changed = data.issendalive!=i;
         data.aliveinterval=i;
         return true;
    }

    bool setIsStopalive(bool i, bool* changed = nullptr){
        if(changed) *changed = data.isstopalive!=i;
        data.isstopalive=i;
        return true;
    }
    bool setIsStopserver(bool i, bool* changed = nullptr){
        if(changed) *changed = data.isstopserver!=i;
        data.isstopserver=i;
        return true;
    }

    int threads(){return data.threads;}
    QString aliveaddr(){return data.aliveaddr;}
    bool issendalive(){return data.issendalive;}
    int aliveinterval(){return data.aliveinterval;}
    int ptime(){return data.ptime;}
    bool isstopalive(){return data.isstopalive;}
    bool isstopserver(){return data.isstopserver;}
    const QString& user() const {return data.user;}
    const QString& group() const {return data.group;}
    static bool save(const QString &txt, const QString &fn, bool isAppend=false);
private:
    QMap<QString, QString> LoadIni(const QString& fn);
    bool SaveIni(const QString &fn, const QMap<QString, QString> &d);

    bool get(int* a, const QString& k, const QMap<QString,QString>& d);
    bool get(qreal* a, const QString &k, const QMap<QString,QString> &d);
    bool get(QString* a, const QString &k, const QMap<QString,QString> &d);
    bool get(bool* a, const QString& k, const QMap<QString,QString>& d);

    void set(int a, const QString& k, QMap<QString,QString>& d);
    void set(const QString &v, const QString &k, QMap<QString,QString> &d);
    void set(bool a, const QString& k, QMap<QString,QString>& d);
    void set(qreal a, const QString& k, QMap<QString,QString>& d);

};



#endif // SETTINGS_H
