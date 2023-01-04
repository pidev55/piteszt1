#include "httpthreadedserver.h"
#include "common/logger/log.h"
#include "response.h"
#include "zactionhelper.h"
//#include <QTextCodec>
#include "zthreadedsockethandler.h"
#include <QThreadPool>
#include "zactiontask.h"

HTTPThreadedServer::HTTPThreadedServer(const QString& name, QObject *parent) : QTcpServer(parent)
{
    QThreadPool::globalInstance()->setMaxThreadCount(5);
    this->_serverName = name;    
}

void HTTPThreadedServer::setHostAddress(const QHostAddress& a, quint16 p)
{
    this->host = a;
    this->port = p;
}

void HTTPThreadedServer::start()
{
    auto isok = listen(host,port);
    isPaused = false;
    if(isok)
    {
        zInfo(QStringLiteral("Web server:%3 is waiting for a connection at %1:%2").arg(host.toString()).arg(port).arg(_serverName));
    }
    else
    {
        zInfo(QStringLiteral("Web server could not start at %1:%2").arg(host.toString()).arg(port));
    }
}

void HTTPThreadedServer::stop()
{
    this->close();
    zInfo(QStringLiteral("Web server:%1 is closed").arg(_serverName));
}

void HTTPThreadedServer::incomingConnection(qintptr socketDescriptor)
{
    zInfo("incoming:"+QString::number(socketDescriptor));
    // Amikor bejön egy connection, csinálunk egy klienst , ami amúgy nem kliens, inkább socket handler
    auto *client = new zThreadedSocketHandler(this);
    client->setSocket(socketDescriptor);
    //client->setServer(this);
    connect(client, &zThreadedSocketHandler::socketready, this, &HTTPThreadedServer::on_socketready);
    //connect(client, SIGNAL(socketready(const QByteArray& r, qintptr desc)), this, SLOT(on_socketready(const QByteArray& r, qintptr desc)));
    _clients.append(client);
}

zThreadedSocketHandler* HTTPThreadedServer::GetClient(qintptr desc)
{
    for(auto&a:_clients)
    {
        if(a->socketDescriptor()==desc)
            return a;
    }
    return nullptr;
}

void HTTPThreadedServer::Disconnected(qintptr desc)
{
    auto b = GetClient(desc);

    if(b)
    {
        _clients.removeOne(b);
        delete b;
    }
}

void HTTPThreadedServer::addAction(QString m, QString p, actionFn fn)
{
    if(m.isEmpty()) return;
    if(p.isEmpty()) return;
    if(fn == nullptr) return;
    if(!p.startsWith('/')) p='/'+p;

    bool isContains = zActionHelper::contains(this->actions, m, p);

    if(isContains)
    {
        zInfo(QStringLiteral("action already added: %1 %2").arg(m, p));
    }
    else
    {
        Action a(m, p, fn);
        this->actions.append(a);
    }
}

const Action* HTTPThreadedServer::action(Request r)
{
    return zActionHelper::find(actions, r);
}

// miután a task elkészült
// az eredményt válasszá alakítjuk
void HTTPThreadedServer::TaskResult(ActionResult e, qintptr desc)
{
    Response rs;

    rs.setStatus(ActionResult::Status::Codes::OK);
    rs.addBody(e);
    rs.addHeaderField(Response::headerField::Server, this->serverName());

    sendResponse(rs, desc);
}

void HTTPThreadedServer::sendResponse(const Response& rs, qintptr desc){
    auto c = GetClient(desc);
    if(c) c->sendResponse(rs);
}


void HTTPThreadedServer::notFound(qintptr desc)
{
    Response rs;

    rs.setStatus(ActionResult::Status::Codes::NotFound);
    rs.addHeaderField(Response::headerField::Server, this->serverName());

    sendResponse(rs, desc);
}

void HTTPThreadedServer::startTask(const Request& r, const Action action, qintptr desc){
    zInfo(QStringLiteral("*** START_TASK(%1) -> %2").arg(r.instance).arg(r.URI));
    zActionTask *mytask = new zActionTask();

    mytask->setActionFn(action.fn, r.urlparams, r.content, desc);
    mytask->setAutoDelete(true);

    // sorba állítjuk
    //connect(mytask, SIGNAL(Result(ActionResult)), this, SLOT(TaskResult(ActionResult)), Qt::QueuedConnection);

    connect(mytask, &zActionTask::Result, this, &HTTPThreadedServer::TaskResult);
    QThreadPool::globalInstance()->start(mytask);
}

void HTTPThreadedServer::on_socketready(const QByteArray& ba, qintptr descriptor)
{
    Request r(ba);
    if(r.status==1)
    {
        if(r.isCompleted())
        {
            auto action = this->action(r);
            if(action !=nullptr)
            {
                startTask(r, *action, descriptor);
            }
            else
            {
                notFound(descriptor);
            }
        }
        else
        {
            this->pending_requests[descriptor]=r;
        }
    }
    else
    {
        if(this->pending_requests.contains(descriptor))
        {
            auto& r2 = this->pending_requests[descriptor];
            r2.content+=ba;

            QString msg = "  "+QString::number(r2.content.length())+" bytes";
            zInfo("APPEND_CONTENT "+r2.URI+msg);

            if(r2.isCompleted())
            {
                zInfo("COMPLETE");

                this->pending_requests.remove(descriptor);
                auto action = this->action(r2);
                if(action != nullptr)
                {
                    startTask(r2, *action, descriptor);
                }
                else
                {
                    notFound(descriptor);
                }
            }
            else{
                zInfo("NOT_COMPLETE");
            }
        }
        else
        {
            Response rs;
            rs.setStatus(ActionResult::Status::Codes::OK);
            rs.addHeaderField(Response::headerField::Server, this->serverName());
            //rs.addHeaderField(zResponse::headerField::ContentType, "text/html; charset=UTF-8");
            rs.addBody("");

            sendResponse(rs, descriptor);
        }
    }


}
