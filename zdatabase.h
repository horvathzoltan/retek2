#ifndef ZDATABASE_H
#define ZDATABASE_H

#include <QString>

class zDataBase
{
public:
    zDataBase();

    static bool Connect(QString connectionString, QString u, QString p);
};

#endif // ZDATABASE_H
