#ifndef ZSTRINGHELPER_H
#define ZSTRINGHELPER_H

#include <QString>

class zStringHelper
{
public:
    zStringHelper();
    static bool toBool(QString);  

    static QStringList toStringList(QString s);
};

#endif // ZSTRINGHELPER_H
