#ifndef ZTABLEERROR_H
#define ZTABLEERROR_H

#include <QString>

struct zTableError
{
public:
    QString tableName;
    QString rowName;
    QString colName;
    QString message;
    static zTableError Parse(const QString& m);
    QString toString();

    bool isValid();
private:
    static const QString colNamePattern;
    static const QRegularExpression colNameRegexp;


};

#endif // ZTABLEERROR_H
