#ifndef ZTABLEROW_H
#define ZTABLEROW_H


#include <QString>
#include <QXmlStreamWriter>

class zTablerow
{
public:
    zTablerow();
    zTablerow(const QString &colName, const QString &dtype, int dlen, bool isNullable, const QString &caption);

    QString Caption;
    QString colName;
    QString colType;
    int dlen;
    bool isNullable;
    QString comment;

    QString toString();

    bool operator==(const zTablerow&) const;

    static zTablerow* getByName(QList<zTablerow>*, const QString&);

    QList<QString> Validate(zTablerow*);
    QString CompareCaption(const QString&);
    QString CompareColType(const QString&);
    QString ValidateNullable(bool);
    QString ValidateDLen(int);
    void toXML(QXmlStreamWriter*);
    static zTablerow fromXML(QXmlStreamReader*);
};

#endif // ZTABLEROW_H
