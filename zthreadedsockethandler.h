#ifndef MYCLIENT_H
#define MYCLIENT_H

//#include <QObject>
#include <QTcpSocket>
//#include <QDebug>
//#include <QThreadPool>
//#include "zactiontask.h"
#include "actionresult.h"
//#include "httpthreadedserver.h"
#include "response.h"

class zThreadedSocketHandler : public QObject
{
    Q_OBJECT
public:
    explicit zThreadedSocketHandler(QObject *parent = nullptr);
    ~zThreadedSocketHandler();
    void setSocket(qintptr Descriptor);
    //void setServer(HTTPThreadedServer *s);
    void trace(QString);
    void sendResponse(Response e);
    qintptr socketDescriptor()
    {
        if(!socket) return -1;
      //  if(isDeleting) return -1;
        return socket->socketDescriptor();
    }

    //bool isDeleting=false;
public slots:
    void connected();
    void disconnected();
    void readyRead();

signals:
    void socketready(const QByteArray& r, qintptr desc);


    // make the server fully ascynchronous
    // by doing time consuming task

private:
    static const QString LOGPATTERN;

    QTcpSocket *socket;
    //HTTPThreadedServer *server;


};

#endif // MYCLIENT_H
