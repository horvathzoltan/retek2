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

//    static QString singularize(QString s);
//    static QString pluralize(QString s);

//    static QString singularizeAll(QString s);

    static QString getClassNameCamelCase(QString tablanev);
    static bool isClassName(QString str);    
    static QRegularExpression r_string_or_number;
    static QString caseFixer(QString, QString);
};

#endif // ZSTRINGHELPER_H
