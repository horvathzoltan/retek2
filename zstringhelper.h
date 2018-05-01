#ifndef ZSTRINGHELPER_H
#define ZSTRINGHELPER_H

#include <QString>

class zStringHelper
{
public:
    zStringHelper();
    static bool toBool(QString);  

    static QString toCamelCase(QString);
    static QStringList toStringList(QString s);

    static QString singularize(QString s);
    static QString pluralize(QString s);

    static QString singularizeAll(QString s);

    static QString getClassNameCamelCase(QString tablanev);
};

#endif // ZSTRINGHELPER_H
