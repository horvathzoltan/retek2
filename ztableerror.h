#ifndef ZTABLEERROR_H
#define ZTABLEERROR_H

#include "zstringhelper.h"
#include <QStringList>

struct zTableError
{
public:
    QString tableName;
    QString rowName;
    QString colName;    
    QString message;
    QStringList params;
    QString source;
//    static zTableError Parse(const QString& m);
    QString toString() const;

    zTableError(const QString&, const QString& ,const QString&, const QString&,  const QString&, const QString & = zStringHelper::Empty, const QStringList &l = QStringList());

    bool isValid() const;
    static const zTableError Empty;

    void addDescription(const QString&);
private:    
    QString description;

    static const QString colNamePattern;
    static const QRegularExpression colNameRegexp;


};

#endif // ZTABLEERROR_H
