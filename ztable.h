#ifndef ZTABLE_H
#define ZTABLE_H

#include <QString>
#include <QList>

#include "ztablerow.h"

class zTable
{
public:
    zTable(QString tablanev, QList<zTablerow>);
    //~zTable();

    QString tablename;
    QList<zTablerow> rows;

    static zTable LoadFromSQL(QString tablename, QMap<QString, QString> globalCaptionMap, QString fn);

    QString toString();

    QList<QString> Validate(zTable);
};

#endif // ZTABLE_H
