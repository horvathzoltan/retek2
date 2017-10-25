#ifndef ZTABLE_H
#define ZTABLE_H

#include <QString>
#include <QList>

#include "ztablerow.h"

class zTable
{
public:
    zTable(QString tablanev, QList<zTablerow>);

    zTable();
    ~zTable();

    QString tablename;
    QList<zTablerow> rows;

    //static zTable LoadFromSQL(QString, QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMSSQL(QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMySQL(QString, QMap<QString, QString>, QString );

    QString toString();

    QList<QString> Validate(zTable);
};

#endif // ZTABLE_H
