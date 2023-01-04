#ifndef TCPSENDER_H
#define TCPSENDER_H

#include <QString>
#include <QTcpSocket>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QProcess>
#include <QDataStream>
#include <QTimer>
#include <QAbstractSocket>
#include <QDateTime>

class zTcpSocket : public QObject
{
    Q_OBJECT

public:
    static const QString _hw_id;
    static const QString _hw_ip;
    explicit zTcpSocket (QObject *parent = nullptr);
    void init(const QString&, quint16);
    void setFn(bool(*)(const QByteArray&));
    void sendTcpAlive();
    static void sendAlive(void *socket);
    static bool responseOk(const QByteArray&);
    //static bool isPinging;
    bool zSend(const QString &msg);
    static QString hwid();
    static QStringList GetIp(uchar i1, uchar i2, quint16 p);
    static QString host(){ return _host;}
    void timeoutTimerToThread(QThread *t);
    static QString hwid_wlan();
private:
    QTcpSocket *socket;
    bool (*fn)(const QByteArray&);
    bool zSend();
    //void zStateChanged();
    void Error(QAbstractSocket::SocketError);
    QTimer *timeoutTimer;
    //QProcess *ping;
    //QStringList *params;
    static QString _host;
    static QString _original_host;
    quint16 port;
    //static QString hwid();
    static QString hwip(const QString&);
    static void OnlineLed(bool st);
private slots:
    void zConnected();
    void zReadyRead();
    void zTimedOut();
    //void zPingFinished(int exitCode, QProcess::ExitStatus exitStatus);
};
#endif // TCPSENDER_H
