#include "actionresult.h"

Action::Action()
{

}

Action::Action(QString m, QString p, actionFn fn)
{
    this->method = m;
    this->path = p;
    this->fn = fn;
}

const QMap<ActionResult::Status::Codes, QString> ActionResult::Status::statuses {
    {Continue, QStringLiteral("Continue")},
    {SwitchingProtocols, QStringLiteral("Switching Protocols")},

    {OK, QStringLiteral("OK")},
    {Created, QStringLiteral("Created")},
    {Accepted, QStringLiteral("Accepted")},
    {NonAuthoritativeInformation, QStringLiteral("Non-Authoritative Information")},
    {NoContent, QStringLiteral("No Content")},
    {ResetContent, QStringLiteral("Reset Content")},
    {PartialContent, QStringLiteral("Partial Content")},
    {MultipleChoices, QStringLiteral("Multiple Choices")},
    {MovedPermanently, QStringLiteral("Moved Permanently")},
    {Found, QStringLiteral("Found")},
    {SeeOther, QStringLiteral("See Other")},
    {NotModified, QStringLiteral("Not Modified")},
    {UseProxy, QStringLiteral("Use Proxy")},
    {TemporaryRedirect, QStringLiteral("Temporary Redirect")},

    {BadRequest, QStringLiteral("Bad Request")},
    {Unauthorized, QStringLiteral("Unauthorized")},
    {PaymentRequired, QStringLiteral("Payment Required")},
    {Forbidden, QStringLiteral("Forbidden")},
    {NotFound, QStringLiteral("Not Found")},
    {MethodNotAllowed, QStringLiteral("Method Not Allowed")},
    {NotAcceptable, QStringLiteral("Not Acceptable")},
    {ProxyAuthenticationRequired, QStringLiteral("Proxy Authentication Required")},
    {RequestTimeout, QStringLiteral("Request Time-out")},
    {Conflict, QStringLiteral("Conflict")},
    {Gone, QStringLiteral("Gone")},
    {LengthRequired, QStringLiteral("Length Required")},
    {PreconditionFailed, QStringLiteral("Precondition Failed")},
    {RequestEntityTooLarge, QStringLiteral("Request Entity Too Large")},
    {RequestURItooLarge, QStringLiteral("Request-URI Too Large")},
    {UnsupportedMediaType, QStringLiteral("Unsupported Media Type")},
    {RequestedRangeNotSatisfiable, QStringLiteral("Requested range not satisfiable")},
    {ExpectationFailed, QStringLiteral("Expectation Failed")},

    {InternalServerError, QStringLiteral("Internal Server Error")},
    {NotImplemented, QStringLiteral("Not Implemented")},
    {BadGateway, QStringLiteral("Bad Gateway")},
    {ServiceUnavailable, QStringLiteral("Service Unavailable")},
    {GatewayTimeout, QStringLiteral("Gateway Time-out")},
    {HTTPversionNotSupported, QStringLiteral("HTTP Version not supported")}
    };

