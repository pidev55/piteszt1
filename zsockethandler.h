#ifndef ZSOCKETHANDLER_H
#define ZSOCKETHANDLER_H

#include <QTcpSocket>
#include "httpthreadedserver.h"


class zSocketHandler : public QObject
{
    Q_OBJECT
public:
    explicit zSocketHandler(QObject *parent = nullptr);

    HTTPThreadedServer *server;
    QTcpSocket *socket;
    void setServer(HTTPThreadedServer *s);
public slots:
    void txRx();
    void closingClient();
};

#endif // ZSOCKETHANDLER_H
