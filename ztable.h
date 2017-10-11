#ifndef ZTABLE_H
#define ZTABLE_H

#include <QString>
#include <QVector>

#include "ztablerow.h"

class zTable
{
public:
    zTable(QString tablanev);
    //~zTable();

    QString tablename;
    QVector<zTablerow> rows;

    static zTable LoadFromSQL(QString tablename, QMap<QString, QString> globalCaptionMap, QString fn);
};

#endif // ZTABLE_H
