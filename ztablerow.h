#ifndef ZTABLEROW_H
#define ZTABLEROW_H


#include <QString>

class zTablerow
{
public:
    zTablerow();
    zTablerow(QString colName, QString dtype, int dlen, bool nullable, QString caption);

    QString Caption;
    QString colName;
    QString colType;
    int dlen;
    bool nullable;
};

#endif // ZTABLEROW_H
