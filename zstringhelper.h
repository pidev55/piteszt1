#ifndef ZSTRINGHELPER_H
#define ZSTRINGHELPER_H
#include <QString>

class zStringHelper
{
public:

    static const char * toCstr(const QString& s);
};

#endif // ZSTRINGHELPER_H
