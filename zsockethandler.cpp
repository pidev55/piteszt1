#include "request.h"
#include "response.h"
#include "zsockethandler.h"
#include "zactionhelper.h"

zSocketHandler::zSocketHandler(QObject *parent): QObject(parent)
{

}

void zSocketHandler::setServer(HTTPThreadedServer *s)
{
    this->server = s;
}


void zSocketHandler::txRx()
    {
    char webBrowerRXData[1000];
    qint64 sv=socket->read(webBrowerRXData,1000);
    //zlog.trace("reading web browser data");
    QString a(webBrowerRXData);

    Request r(a);

    //zlog.trace(r.toString());

    //auto ac = zActionHelper::find(this->actions, r);
    auto ac = server->action(r);

    Response rs;
    if(ac != nullptr)
    {
        rs.setStatus(ActionResult::Status::Codes::OK);
        //QString q = r.url.query();
        auto eredmeny = ac->fn(r.urlparams, r.content);
        rs.addBody(eredmeny);
    }
    else
    {
        rs.setStatus(ActionResult::Status::Codes::NotFound);
    }

    rs.addHeaderField(Response::headerField::Server, server->serverName());
    socket->write(rs.toByteArray());
    socket->disconnectFromHost();
}

void zSocketHandler::closingClient()
{
        socket->deleteLater();
        delete this;
}
