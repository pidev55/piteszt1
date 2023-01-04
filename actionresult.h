#ifndef ACTION_H
#define ACTION_H

#include "common/macrofactory/macro.h"

#include <QUrlQuery>


// ez történik amikor egy kérés befut a szerverhez
// és a kérés methodhoz és url-hez van társítva egy függvény

struct ActionResult{
public:
    enum Type{HTML, TEXT, JSON, IMAGE};
struct Status
    {
public:
    enum Codes: int {
        //Informational
        Continue=100,
        SwitchingProtocols=101,
        //Successful
        OK=200,
        Created=201,
        Accepted=202,
        NonAuthoritativeInformation=203,
        NoContent=204,
        ResetContent=205,
        PartialContent=206,
        //Redirection;
        MultipleChoices=300,
        MovedPermanently=301,
        Found=302,
        SeeOther=303,
        NotModified=304,
        UseProxy=305,
        TemporaryRedirect=307,
        //Client Error
        BadRequest=400,
        Unauthorized=401,
        PaymentRequired=402,
        Forbidden=403,
        NotFound=404,
        MethodNotAllowed=405,
        NotAcceptable=406,
        ProxyAuthenticationRequired=407,
        RequestTimeout=408,
        Conflict=409,
        Gone=410,
        LengthRequired=411,
        PreconditionFailed=412,
        RequestEntityTooLarge=413,
        RequestURItooLarge=414,
        UnsupportedMediaType=415,
        RequestedRangeNotSatisfiable=416,
        ExpectationFailed=417,
        //Server Error
        InternalServerError=500,
        NotImplemented=501,
        BadGateway=502,
        ServiceUnavailable=503,
        GatewayTimeout=504,
        HTTPversionNotSupported=505
    };

private:
    static const QMap<Codes, QString> statuses;
public:
    static QString statusPhrase(Codes s){ return statuses.value(s, QStringLiteral("Unknown")); }

    static QString getStatusPhrase(Codes s){return QStringLiteral("%1: %2").arg(s).arg(ActionResult::Status::statusPhrase(s));}
    };
private:
    QByteArray _content;
    Type _type;
    Status::Codes _status;
public:
    ActionResult(){}
    ActionResult(const QByteArray& b, Type t, Status::Codes c = Status::OK){
        this->_content = b;
        this->_type = t;
        this->_status = c;
    }

    ActionResult(const QString & b, Status::Codes code = Status::OK) :
        ActionResult(b.toUtf8(), Type::TEXT, code){}

    bool isValid(){return !this->_content.isEmpty(); }
    static ActionResult Error(const QString& s, Status::Codes code = Status::BadRequest) {return {QStringLiteral("error:")+s, code};}
    const QByteArray& content() {return _content;}
    QString type(){
                static const auto _TEXT = nameof(TEXT);
                static const auto _JSON = nameof(JSON);
                static const auto _IMAGE = nameof(IMAGE);
                static const auto _HTML = nameof(HTML);

                switch(_type)
                {
                case TEXT: return _TEXT;
                case JSON: return _JSON;
                case IMAGE: return _IMAGE;
                default: return _HTML;
                }
    }
    ActionResult::Status::Codes status(){return _status;}

    ActionResult append(const QString& e) const { ActionResult a = *this; a._content.append('\n'+e.toUtf8()); return a; }
};

Q_DECLARE_METATYPE(ActionResult);

typedef ActionResult (*actionFn)(const QUrlQuery&, const QByteArray&);

class Action
{
public:
    Action();

    Action(QString, QString, actionFn);

    QString method;
    QString path;
    actionFn fn;
};



#endif // ACTION_H
