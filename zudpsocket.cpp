#include "zudpsocket.h"
#include "common/logger/log.h"
#include "command.h"

zUdpSocket::zUdpSocket(QObject *parent):QObject(parent)
{
    socket = new QUdpSocket(this);
    socket->bind(1998, QUdpSocket::ShareAddress);
    connect(socket, &QUdpSocket::readyRead, this, &zUdpSocket::zReadyRead);
    //socket2 = nullptr;
    //socket2 = new QTcpSocket(this);
    zInfo("UDP:1998,TCP:1999")
}

zUdpSocket::~zUdpSocket()
{
    socket->close();
    socket->deleteLater();
}

void zUdpSocket::zReadyRead()
{
    QList<QByteArray> datagrams;

    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(int(socket->pendingDatagramSize()));
        socket->readDatagram(datagram.data(), datagram.size());
        datagrams.append(datagram);
    }

    static quint32 n=0;
    for(auto&d : datagrams){
        int i = Command::doWork(n++, d, nullptr);
        if(i==-1){
            zInfo(QStringLiteral("command not found: %1").arg(d[0]));
        }
    }
}
