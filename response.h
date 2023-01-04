#ifndef RESPONSE_H
#define RESPONSE_H

#include <QMap>
#include "actionresult.h"

class Response
{
public:
    Response();

    struct headerField
    {
        static const QString ContentType;
        static const QString Connection;
        static const QString Server;
        static const QString ContentLength;
        static const QString CORS;
    };


    //Response(int, const QByteArray&);
    QByteArray toByteArray();
    void setStatus(ActionResult::Status::Codes s);
    void addBody(const QByteArray& b);
    void addBody(ActionResult& a);
    void addHeaderField(const QString& k, const QString& v);
    //https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.2.1
    // van válasz

    //QString getStatusPhrase();

    static const QString HTTPversion;
    static const QString keyValuePattern;
    static const QString statusLinePattern;

    ActionResult::Status::Codes status(){return _status;}

private:
    //static const QMap<int, QString> statuses;

    ActionResult::Status::Codes _status;
    QByteArray _body;
    QMap<QString, QString> _headerFields;
};

#endif // RESPONSE_H
