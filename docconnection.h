#ifndef DOCCONNECTION_H
#define DOCCONNECTION_H
#include <QString>

class docConnection
{
public:
    QString Name;
    QString path;

    docConnection(QString _name, QString _path);
    static docConnection FromCSV(const QString&);
    QString toCSV() const ;
    static QList<docConnection> fromCSVFile(const QString& f);
    bool isValid();
};

#endif // DOCCONNECTION_H
