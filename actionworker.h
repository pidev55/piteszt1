#ifndef ACTIONWORKER_H
#define ACTIONWORKER_H
#include "actionresult.h"

typedef ActionResult (*ActionWorker)(const QUrlQuery& param, const QByteArray& content);

#endif // ACTIONWORKER_H
