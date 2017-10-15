#ifndef ZDATABASE_H
#define ZDATABASE_H

#include <QString>

class zDataBase
{
public:
    zDataBase();

    static const QString connectionTemplate;

    static QString getConnStr(QString driverName, QString serverName, QString dbName);

    static bool Connect(QString driver, QString hostname, QString databasename, QString u, QString p);
};



#endif // ZDATABASE_H
