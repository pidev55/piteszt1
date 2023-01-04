#ifndef REQUEST_H
#define REQUEST_H

#include <QMap>
#include <QString>
#include <QUrl>
#include <QUrlQuery>

/*
https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
*/

class Request
{    
public:
    struct Method {
        static const QString OPTIONS;
        static const QString GET;
        static const QString HEAD;
        static const QString POST;
        static const QString PUT;
        static const QString DELETE;
        static const QString TRACE;
        static const QString CONNECT;
    };

    Request();
    ~Request();
    Request(QString);

    bool isCompleted();
    //Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
    QString method;
    QString URI;
    QString HTTPversion;
    QUrl url;
    QUrlQuery urlparams;
    QMap<QString, QString> headerFields;
    QByteArray content;
    int content_length;
    int status;
    int instance;

    QString toString();
private:

    static int instance_counter;
};

#endif // REQUEST_H

