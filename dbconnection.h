#ifndef DBCONNECTION_H
#define DBCONNECTION_H
#include <QString>

class dbConnection
{
private:

public:    
    QString driver;
    QString schemaName;
    QString server;
    QString user;
    QString password;

    dbConnection(QString, QString, QString, QString, QString);

    QString ToCSV();
    static dbConnection FromCSV(QString);

    bool isValid();

    bool operator==(dbConnection const& b);

    QString Getname();
};


#endif // DBCONNECTION_H
