#ifndef ZTABLE_H
#define ZTABLE_H

#include <QString>
#include <QList>

#include "ztablerow.h"

class zTable
{
public:
    zTable(QString tablanev, QString pkn, QList<zTablerow>);

    zTable();
    ~zTable();

    QString tablename;
    QString pkname;

    QList<zTablerow> rows;

    //static zTable LoadFromSQL(QString, QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMSSQL(QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMySQL(QString, QMap<QString, QString>, QString );

    QString toString();

    QList<QString> Validate(zTable);

    static QList<zTable> createTableByText(QString txt);
    static zTable* getByName(QList<zTable> *tables, QString rn);
};

#endif // ZTABLE_H
