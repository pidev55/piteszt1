#include "request.h"
#include "common/logger/log.h"

/*
https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
*/

const QString Request::Method::OPTIONS = QStringLiteral("OPTIONS");
const QString Request::Method::GET = QStringLiteral("GET");
const QString Request::Method::HEAD = QStringLiteral("HEAD");
const QString Request::Method::POST = QStringLiteral("POST");
const QString Request::Method::PUT = QStringLiteral("PUT");
const QString Request::Method::DELETE = QStringLiteral("DELETE");
const QString Request::Method::TRACE = QStringLiteral("TRACE");
const QString Request::Method::CONNECT = QStringLiteral("CONNECT");

int Request::instance_counter=0;

Request::Request()
{
    zInfo(QStringLiteral("CONSTRUCT(%1)").arg(instance))//.arg(a)
    this->instance= instance_counter++;
    zInfo(QStringLiteral("CONSTRUCT(%1)").arg(instance))//.arg(a)
}

Request::~Request()
{
    zInfo(QStringLiteral("DESTRUCT(%1)").arg(instance))//.arg(a)
}

Request::Request(QString r)
{
    zInfo(QStringLiteral("CONSTRUCT(%1)2-1"))
    this->instance= instance_counter++;
    zInfo(QStringLiteral("CONSTRUCT(%1)2").arg(instance))//.arg(a)

    this->status=1;
    this->content_length=-1;

    auto lines = r.split("\r\n");
    QString requestLine = lines[0];

    auto rd = r;

    if(!requestLine.isEmpty())
    {
        auto requestLineParts = requestLine.split(' ');

        if(requestLineParts.count()==3)
        {
            this->method = requestLineParts[0];
            this->URI = requestLineParts[1];
            this->HTTPversion = requestLineParts[2];

            if(!URI.isEmpty())
            {
                this->url = QUrl(URI);                
                this->urlparams = QUrlQuery(url);
                this->content = lines.last().toUtf8();
            }            
        }
        else
        {
            zError("requestLine inappropriate format");
            this->status=0;
            return;
        }

        if(lines.count()>1)
        {
            for(int i = 1;i<lines.count();i++)
            {
                QString line = lines[i];
                if(line.isEmpty()) break;
                auto ix = line.indexOf(':');

                if(ix>-1)//és ha 0, akkor nincs kulcs...
                {
                    if(ix>0)
                    {
                        this->headerFields.insert(line.left(ix),line.mid(ix+1));
                    }
                    else
                    {
                        zError(QStringLiteral("there is no key: %1").arg(line));
                        //this->status=0;
                    }

                }
                else
                {
                    zError(QStringLiteral("headerFieldLine inappropriate format: %1").arg(line));
                    //this->status=0;
                }
            }
        }
    }    
}

QString Request::toString()
{
    auto e = QStringLiteral("%1 %2 %3").arg(this->method, this->URI, this->url.toString());
    return e;
}

bool Request::isCompleted(){
    auto hasBody = this->headerFields.contains("Content-Length");
    if(hasBody)// && (r.content.isEmpty() || r.content.length()<))
    {
        int l = this->headerFields["Content-Length"].toInt();
        if(this->content.length()<l)
        {
            //részleges request - csak a fej van meg
            zInfo(QStringLiteral("partially %1 , %2").arg(l).arg(this->content.length()))

            return false;
        }
    }
    return true;
}



