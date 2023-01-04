#ifndef GLOBALS_H
#define GLOBALS_H

//#include <QRegularExpression>
//#include "httpthreadedserver.h"
//#include "pressurehelperevent.h"
//#include "restarter.h"
//#include "updater.h"
//#include "zlogicexception.h"

//extern HTTPThreadedServer *server;
//extern Restarter restarter;
//extern Updater updater;
//extern PressureHelperEvent events;

#ifndef GPIOPINDEFINES_H
#define GPIOPINDEFINES_H

#define LED_REC 0 // 11 láb gpio17
#define LED_API 2 // 13 gpio27
#define LED_ALIVE 3 // 15 gpio22

#endif // GPIOPINDEFINES_H
/*#define zforeach(var, container) for(auto (var) = (container).begin(); (var) != (container).end(); ++(var))
#define zforeach_from(var, container, ix) for(auto (var) = (container).begin()+(ix); (var) != (container).end(); ++(var))

#define nameof(x) z_macro_factory::_nameof<0>(#x, sizeof(x))
#define zfn() z_macro_factory::_zfn<0>((const char*)Q_FUNC_INFO)//Q_FUNC_INFO


namespace z_macro_factory {
    template<int a>
    QString _nameof(const char* y, std::size_t)
    {
        QString x(y);
        //QRegularExpression re("^&?([_a-zA-Z]\\w*(->|\\.|::))*([_a-zA-Z]\\w*)$");
        QRegularExpression re(QStringLiteral(R"(^&?([_a-zA-Z]\w*)\s*(->|\.|::)?\s*([_a-zA-Z]\w*)?$)"));
        QRegularExpressionMatch m = re.match(x);

        if (m.hasMatch()) {
            return m.captured(m.lastCapturedIndex());
        }
        throw zLogicException("A bad expression x in nameof(x). The expression is \"" + x + "\".");
    }

    template<int a> QString _zfn(const char* y)
    {
        QString x(y);

        QRegularExpression re(QStringLiteral(R"(([\w]+::[\w]+))"));
        QRegularExpressionMatch m = re.match(x);

        if (m.hasMatch())
        {
            return m.captured(m.lastCapturedIndex());
        }

        throw zLogicException("A bad expression x in _zfn(x). The expression is \"" + x + "\".");
        return QStringLiteral("unknown");
    }
}// namespace z_macro_factory
*/
#endif // GLOBALS_H

