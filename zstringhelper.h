#ifndef ZSTRINGHELPER_H
#define ZSTRINGHELPER_H

#include <QString>

class zStringHelper
{
public:
    zStringHelper();
    static bool toBool(QString);

    static QString Load(QString filename);
    static void Save(QString *classTxt, QString fn);
};

#endif // ZSTRINGHELPER_H
