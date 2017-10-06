#ifndef ZTABLE_H
#define ZTABLE_H

#include <QString>
#include <QVector>

#include "ztablerow.h"

class zTable
{
public:
    QString tablename;
    QVector<zTablerow> rows;

    static zTable LoadFromSQL(QString tablename);
};

#endif // ZTABLE_H
