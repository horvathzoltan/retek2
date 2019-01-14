#ifndef ZTABLEROW_H
#define ZTABLEROW_H

#include <QString>
#include <QXmlStreamWriter>
#include <QMap>
#include "ztableerror.h"

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

    bool Compare(const zTablerow& rv, QList<zTableError>& e, const QString& source);
    bool Validate(QStringList& e);
    QString CompareCaption(const QString&);
    bool isKnownTypeName(const QString&);
    QString ValidateNullable(bool);
    QString ValidateDLen(int);
    void toXML(QXmlStreamWriter*);
    static zTablerow fromXML(QXmlStreamReader*);
    bool Validate2(const QStringList& colNames, QList<zTableError> &e, const QString &source);
    static const zTablerow* find(const QList<zTablerow>& rows, const QString& rn);
    static int findIx(const QList<zTablerow>& rows, const QString& rn);
    static QStringList colNames(const QList<zTablerow>& rows);

    enum class ErrCode:int;
    static const QMap<ErrCode, QString> ErrCodeNames;
    static const QMap<ErrCode, QString> ErrCodeDescriptions;

    //QString GetErrorMessage(const QString& tn, const QString& cn, ErrCode code);
    zTableError GetFullError(const QString& cn, ErrCode code, const QStringList& p, const QString &source);
    static const ErrCode* GetErrCode(const QString& a);
    static QStringList GetColNameFromErrorMessage(const QString& tn);
};

#endif // ZTABLEROW_H
