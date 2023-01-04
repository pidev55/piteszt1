#include "response.h"
#include "common/logger/log.h"


const QString Response::keyValuePattern QStringLiteral("%1: %2\r\n");
const QString Response::statusLinePattern QStringLiteral("%1 %2 %3\r\n");
const QString Response::HTTPversion QStringLiteral("HTTP/1.1");

const QString Response::headerField::ContentType = QStringLiteral("Content-Type");
const QString Response::headerField::ContentLength = QStringLiteral("Content-Length");
const QString Response::headerField::Connection = QStringLiteral("Connection");
const QString Response::headerField::Server = QStringLiteral("Server");
const QString Response::headerField::CORS = QStringLiteral("Access-Control-Allow-Origin");





Response::Response()
{

}


//Response::Response(int s,const QByteArray& b)
//{
//    this->statusCode = s;
//    this->body =b;
//}

void Response::setStatus(ActionResult::Status::Codes s)
{
    this->_status = s;
}


void Response::addBody(const QByteArray& b)
{
    this->_body.append(b);
}

void Response::addHeaderField(const QString& k, const QString& v)
{
    if(!_headerFields.contains(k))
    {
        _headerFields.insert(k, v);
    }
    else
    {
        zError(QStringLiteral("headerField already exist: %1").arg(k));
    }
}

QByteArray Response::toByteArray()
{
    QByteArray b;

    auto statusLine = statusLinePattern.arg(HTTPversion).arg(_status).arg(
                ActionResult::Status::statusPhrase(_status)).toUtf8();

    b.append(statusLine);

    if(!_body.isEmpty())
    {
        this->addHeaderField(Response::headerField::ContentLength, QString::number(_body.length()));
        this->addHeaderField(Response::headerField::CORS, QString("*"));
    }

    for(auto i = _headerFields.begin();i!=_headerFields.end();i++)
    {
        b.append(keyValuePattern.arg(i.key(), i.value()).toUtf8());
    }

    b.append("\r\n");
    if(!_body.isEmpty())
    {
        b.append(_body);
    }
    return b;
}




//QString Response::statusPhrase(int s)
//{
//    return statuses.value(s, QStringLiteral("Unknown"));
//}

void Response::addBody(ActionResult& a){
    _status = a.status();
    addBody(a.content());
    addHeaderField(Response::headerField::ContentType, a.type()+" charset=utf-8");
}
