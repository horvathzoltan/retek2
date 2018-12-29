#ifndef SRCCONNECTION_H
#define SRCCONNECTION_H
#include <QString>

class srcConnection
{
public:
    QString Name;
    QString path;

    srcConnection(QString _name, QString _path);
    static srcConnection FromCSV(const QString&);
    QString toCSV() const ;
    static QList<srcConnection> fromCSVFile(const QString& f);
    bool isValid();
};

#endif // SRCCONNECTION_H
