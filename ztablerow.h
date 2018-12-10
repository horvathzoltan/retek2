#ifndef ZTABLEROW_H
#define ZTABLEROW_H


#include <QString>
#include <QXmlStreamWriter>
#include <QMap>

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

    bool Compare(const zTablerow& rv, QStringList& e);
    bool Validate(QStringList& e);
    QString CompareCaption(const QString&);
    bool isKnownTypeName(const QString&);
    QString ValidateNullable(bool);
    QString ValidateDLen(int);
    void toXML(QXmlStreamWriter*);
    static zTablerow fromXML(QXmlStreamReader*);
    bool Validate2(const QStringList& colNames);
    static const zTablerow* find(const QList<zTablerow>& rows, const QString& rn);
    static int findIx(const QList<zTablerow>& rows, const QString& rn);
    static QStringList colNames(const QList<zTablerow>& rows);

    enum class ErrCode:int{noteq, unknown};

    static const QMap<ErrCode, QString> ErrCodeNames;
    static const QMap<ErrCode, QString> ErrCodeDescriptions;

    QString GetErrorMessage(const QString& tn, const QString& cn, ErrCode code);    
    static const ErrCode* GetErrCode(const QString& a);
};

#endif // ZTABLEROW_H
