#include "pressurehelper.h"
#include "ztcpsocket.h"
#include "common/logger/log.h"
#include <QNetworkInterface>



const QString zTcpSocket::_hw_id = hwid_wlan();
const QString zTcpSocket::_hw_ip = hwip(_hw_id);
QString zTcpSocket::_host = {};
QString zTcpSocket::_original_host ={};

void zTcpSocket::setFn(bool (*resultHandler)(const QByteArray &))
{
    fn = resultHandler;
}

zTcpSocket::zTcpSocket(QObject *parent) : QObject(parent),
                                          socket(new QTcpSocket),
                                          timeoutTimer(new QTimer)
                                          /*ping(new QProcess),*/
                                          /*params(new QStringList)*/
{
    zTrace();
    //timeoutTimer->moveToThread(parent->thread());
    //socket->moveToThread(parent->thread());
    timeoutTimer->setSingleShot(true);
    connect(socket, &QTcpSocket::connected, this, &zTcpSocket::zConnected);
    connect(socket, &QTcpSocket::readyRead, this, &zTcpSocket::zReadyRead);
    connect(timeoutTimer, &QTimer::timeout, this, &zTcpSocket::zTimedOut);
    //connect(socket, &QTcpSocket::stateChanged, this, &zTcpSocket::zStateChanged);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this,
            &zTcpSocket::Error);
    //connect(socket, &QTcpSocket::errorOccurred, this, &zTcpSocket::Error);
    //connect(ping, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &zTcpSocket::zPingFinished);
}

QString zTcpSocket::hwid_wlan(){
    auto a= QNetworkInterface::allInterfaces();
    for(auto&i:a) if(i.name()=="wlan0") return i.hardwareAddress();
    return QString();
}

QString zTcpSocket::hwid(){
    auto a= QNetworkInterface::allInterfaces();
    for(auto&i:a)
    {
        if(i.type()==QNetworkInterface::Ethernet) return i.hardwareAddress();
    }
    return QString();
}

QString zTcpSocket::hwip(const QString& hwid){
    auto a= QNetworkInterface::allInterfaces();
    for(auto&i:a)
    {
        if(i.hardwareAddress()==hwid){
            auto addrs = i.allAddresses();
            for(auto&j:addrs){
                if(j.isLoopback()) continue;
                if(j.protocol()==QAbstractSocket::IPv4Protocol) return j.toString();
            }
        }
    }
    return QString();
}

void zTcpSocket::init(const QString & host, quint16 port)
{
    this->_host = host;
    this->_original_host = host;
    this->port = port;

    //params->append("-c1");
    //params->append("-W1");
    //params->append(_host);
}

void zTcpSocket::sendAlive(void* socket){
    if(socket) (reinterpret_cast<zTcpSocket*>(socket))->sendTcpAlive();
}

void zTcpSocket::timeoutTimerToThread(QThread* t){
    this->moveToThread(t);
    timeoutTimer->moveToThread(t);
    socket->moveToThread(t);
}

void zTcpSocket::sendTcpAlive()
{
    OnlineLed(true);
    if(socket->state() == QAbstractSocket::SocketState::UnconnectedState){
        socket->connectToHost(_host, port, QIODevice::ReadWrite);
        timeoutTimer->start(200);
    }
}

bool zTcpSocket::responseOk(const QByteArray &response)
{
    return response.toUpper().startsWith("OK");
}

//bool zTcpSocket::zSend()
//{
//    auto msg = QString(QDateTime::currentDateTimeUtc()
//                           .toString(Qt::DateFormat::ISODateWithMs)
//                       +','
//                       +_hw_id
//                       +",ENDL");

//    QByteArray a = msg.toUtf8();

//    socket->write(a);
//    socket->flush();
//    return true;
//}

bool zTcpSocket::zSend()
{
    auto v0 = PressureHelper::get0();//a csatorna érték
    auto batt = PressureHelper::getBatt();//
    auto msg =
            QDateTime::currentDateTimeUtc().toString(Qt::DateFormat::ISODateWithMs)+','+
            _hw_id+','+
            QString::number(v0)+','+
            QString::number(batt)+','+
            "ENDL";
    QByteArray a = msg.toUtf8();
    socket->write(a);
    socket->flush();
    return true;
}

bool zTcpSocket::zSend(const QString& msg)
{
    QByteArray a = msg.toUtf8();

    socket->write(a);
    socket->flush();
    return true;
}

void zTcpSocket::Error(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    socket->abort();
    OnlineLed(false);
}

void zTcpSocket::zConnected()
{
    timeoutTimer->stop();
    zSend();
    timeoutTimer->start(200);
}

void zTcpSocket::zReadyRead()
{
    timeoutTimer->stop();
    auto resp = socket->readAll();
    socket->close();
    auto st = fn(resp);
    OnlineLed(st);
}

void zTcpSocket::zTimedOut()
{
    OnlineLed(false);
    socket->abort();
    // ezt csak akkor, ha dynamicban van, külömben összeakad a két cpu
    // -  azaz ha a hálón csak egy cpu van, ha több van, nem eldönthető, akkor marad az alap
    // ha a sajátja kimegy, átáll a másikra, de nem jön vissza
//    auto a = GetIp(2,254, 12312);
//    if(a.size()==1) _host= a[0]; else _host = _original_host;
}

//void zTcpSocket::zPingFinished(int exitCode, QProcess::ExitStatus exitStatus)
//{
//    if(exitStatus == QProcess::NormalExit && exitCode == 0)
//    {
//        //socket->abort();
//        //socket->connectToHost(host, port, QIODevice::ReadWrite);
//        //socket->waitForConnected();
//        //auto sstate = socket->state();
//        //if(sstate!= QAbstractSocket::ConnectedState) zInfo("nemjóconn");
//    }
//    else
//    {
//        socket->abort();
//    }
////    else
//    OnlineLed(false);
//    isPinging= false;
//}

void zTcpSocket::OnlineLed(bool st){
    Q_UNUSED(st)
//    GuiHelper::setMainOnline(st);
//    //if(st==false)
//    //    zTrace()
//    digitalWrite(LED_ALIVE, st?HIGH:LOW);
}

//12312
auto zTcpSocket::GetIp(uchar i1, uchar i2, quint16 p) -> QStringList
{
    if(i1>0 && i2<255 && i1>=i2) return {};
    QTcpSocket socket;
    QStringList l;
    for(uchar i = i1;i<=i2;i++)
    {
        auto ip = "10.10.10."+QString::number(i);
        socket.connectToHost(ip, p);
        if(socket.waitForConnected(3))
        {
            socket.disconnectFromHost();
            l.append(ip);
        }
    }
    return l;
}
