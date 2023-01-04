#include "zstringhelper.h"
#include <QString>

const char * zStringHelper::toCstr(const QString& s)
{
    auto p = s.toLocal8Bit().constData();

    //auto p = qPrintable();
    return p;
}
