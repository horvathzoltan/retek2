#ifndef ZTABLEROW_H
#define ZTABLEROW_H


#include <QString>
#include <QXmlStreamWriter>

//enum zRowSearchBy{ Name };

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

    QString toString() const;

    bool operator==(const zTablerow&) const;

    static zTablerow* getByName(const QList<zTablerow>&, const QString&);

    QList<QString> Validate(zTablerow*);
    QString CompareCaption(const QString&);
    bool isKnownTypeName(const QString&);
    QString ValidateNullable(bool);
    QString ValidateDLen(int);
    void toXML(QXmlStreamWriter*);
    static zTablerow fromXML(QXmlStreamReader*);
    bool Validate2(const QStringList& colNames);
    const zTablerow* find(const QList<zTablerow>& rows, const QString& rn);
    static QStringList colNames(const QList<zTablerow>& rows);
};

#endif // ZTABLEROW_H
