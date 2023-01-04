#ifndef ZUDPSOCKET_H
#define ZUDPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>



class zUdpSocket : public QObject
{
    Q_OBJECT

private:
    QUdpSocket *socket;
    //QTcpSocket *socket2;
public:
    zUdpSocket(QObject *parent = nullptr);
    ~zUdpSocket();
private slots:
    void zReadyRead();
};

#endif // ZUDPSOCKET_H
