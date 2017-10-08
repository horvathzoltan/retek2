#ifndef ZTABLE_H
#define ZTABLE_H

#include <QString>
#include <QVector>

#include "ztablerow.h"

class zTable
{
public:
    zTable(QString tablanev);

    QString tablename;
    QVector<zTablerow> rows;

    static zTable LoadFromSQL(QString tablename, QMap<QString, QString> globalCaptionMap, QMap<QString, QString> tablaCaptionMap);
};

#endif // ZTABLE_H
