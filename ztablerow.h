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

    QString toString();

    bool operator==(const zTablerow&) const;

    static zTablerow* getByName(QList<zTablerow>*, QString);

    QList<QString> Validate(zTablerow*);
    QString ValidateCaption(QString);
    QString ValidateColType(QString);
    QString ValidateNullable(bool);
    QString ValidateDLen(int);
};

#endif // ZTABLEROW_H
