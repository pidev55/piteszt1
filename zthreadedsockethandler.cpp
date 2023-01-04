#include "zthreadedsockethandler.h"
#include "common/logger/log.h"
//#include "zrequest.h"
//#include "zactionhelper.h"
//#include "zresponse.h"
//#include <QUrlQuery>
//#include <QThreadPool>
//#include "zactiontask.h"


const QString zThreadedSocketHandler::LOGPATTERN = QStringLiteral("conn %1: %2");

zThreadedSocketHandler::zThreadedSocketHandler(QObject *parent) : QObject(parent)
{
    //QThreadPool::globalInstance()->setMaxThreadCount(5);
}

zThreadedSocketHandler::~zThreadedSocketHandler(){
    //delete socket; socket=nullptr;
    //socket->deleteLater();
}
// amikor a szerver elkap egy bejövő connectiont, csinál egy socketet
// a szervertől kapott descriptoral
void zThreadedSocketHandler::setSocket(qintptr descriptor)
{
    // make a new socket
    socket = new QTcpSocket(this);
    //isDeleting = false;
    socket->setSocketDescriptor(descriptor);

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    trace("socket created:"+QString::number(descriptor));
}


//void zThreadedSocketHandler::setServer(HTTPThreadedServer *s)
//{
//    //this->server = s;
//}

// asynchronous - runs separately from the thread we created
void zThreadedSocketHandler::connected()
{
    //trace(QStringLiteral("connected"));
}

// asynchronous
void zThreadedSocketHandler::disconnected()
{    
    //server->Disconnected(socket->socketDescriptor());
    //delete socket;socket = nullptr;
    connect(socket, &QTcpSocket::destroyed, this, [this]{socket = nullptr;});
    socket->deleteLater();

    //isDeleting = true;
    //trace(QStringLiteral("disconnected"));
}

// ez itt mind a fő szálon történik
// de mi a taskot a poolból vesszük és haszáljuk fel

void zThreadedSocketHandler::readyRead()
{  
    //QByteArray ba;
    auto descriptor = socket->socketDescriptor();

    auto ba = socket->readAll();

    //Request r(ba);

    trace(QString(ba));

    //const zAction* action = nullptr;

    emit socketready(ba, descriptor);
//    if(r.status==1)
//    {
//        if(r.isCompleted())
//        {
//            auto action = server->action(r);
//            if(action !=nullptr)
//            {
//                startTask(r, *action);
//            }
//            else
//            {
//                notFound();
//            }
//        }
//        else
//        {
//            server->pending_requests[descriptor]=r;
//        }
//    }
//    else
//    {
//        if(server->pending_requests.contains(descriptor))
//        {
//            auto r2 = server->pending_requests[descriptor];
//            r2.content+=ba;

//            if(r2.isCompleted())
//            {
//                server->pending_requests.remove(descriptor);
//                auto action = server->action(r2);
//                if(action != nullptr)
//                {
//                    startTask(r2, *action);
//                }
//                else
//                {
//                    notFound();
//                }
//            }
//        }
//        else
//        {
//            Response rs;
//            rs.setStatus(ActionResult::Status::Codes::OK);
//            rs.addHeaderField(Response::headerField::Server, server->serverName());
//            //rs.addHeaderField(zResponse::headerField::ContentType, "text/html; charset=UTF-8");
//            rs.addBody("");

//            socket->write(rs.toByteArray());
//            socket->close();
//            socket->disconnectFromHost();

//        }
//    }

}



// az api válaszát közöljük a klienssel
void zThreadedSocketHandler::sendResponse(Response rs){
    auto a = ActionResult::Status::statusPhrase(rs.status());//rs.status();
    trace(a);
    socket->write(rs.toByteArray());
    socket->close();
    socket->disconnectFromHost();
}

void zThreadedSocketHandler::trace(QString p1)
{
    zInfo(LOGPATTERN.arg(socket->socketDescriptor()).arg(p1));
}

//void zThreadedSocketHandler::startTask(const Request& r, const Action action){
//    zlog.trace(QStringLiteral("TASK(%1)").arg(r.instance));
//    zActionTask *mytask = new zActionTask();
//    //QString q_str = r.url.query();

//    mytask->setActionFn(action.fn, r.urlparams, r.content);
//    mytask->setAutoDelete(true);

//    // sorba állítjuk
//    connect(mytask, SIGNAL(Result(ActionResult)), this, SLOT(TaskResult(ActionResult)), Qt::QueuedConnection);

//    QThreadPool::globalInstance()->start(mytask);
//}


//// miután a task elkészült
//// az eredményt válasszá alakítjuk
//void zThreadedSocketHandler::TaskResult(ActionResult e)
//{
//    Response rs;

//    rs.setStatus(ActionResult::Status::Codes::OK);
//    rs.addBody(e);
//    rs.addHeaderField(Response::headerField::Server, server->serverName());

//    sendResponse(rs);
//}


//void zThreadedSocketHandler::notFound()
//{
//    Response rs;

//    rs.setStatus(ActionResult::Status::Codes::NotFound);
//    rs.addHeaderField(Response::headerField::Server, server->serverName());
//    sendResponse(rs);
//}
