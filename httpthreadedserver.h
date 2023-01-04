#ifndef MYHTTPTHREADSERVER
#define MYHTTPTHREADSERVER

#include <iostream>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include "actionresult.h"
#include "request.h"
#include "zthreadedsockethandler.h"

class HTTPThreadedServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit HTTPThreadedServer(const QString&, QObject *parent = nullptr);
    void setHostAddress(const QHostAddress &, quint16);
    void start();
    void stop();
    void addAction(QString, QString, actionFn);

    bool isPaused = false;
    Q_INVOKABLE void pause(){ isPaused = true;pauseAccepting(); }
    Q_INVOKABLE void resume(){ isPaused = false; resumeAccepting(); }

    const Action* action(Request r);
    const QString serverName(void) const { return _serverName; }
    //QList<zRequest> pending_requests;
    QMap<qintptr, Request> pending_requests;

    void Disconnected(qintptr socketDescriptor);

    void TaskResult(ActionResult e, qintptr desc);
    void notFound(qintptr desc);
    void startTask(const Request& r, const Action action, qintptr desc);

public slots:
    void on_socketready(const QByteArray& r, qintptr desc);

protected:
    void incomingConnection(qintptr socketDescriptor);

    zThreadedSocketHandler* GetClient(qintptr desc);

    QHostAddress host;
    quint16 port;
    QString _serverName;
    QList<Action> actions;

    QList<zThreadedSocketHandler*> _clients;

    void sendResponse(const Response& r, qintptr desc);
//public slots:
//    void Disconn();
};
#endif // MYHTTPTHREADSERVER
