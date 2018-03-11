#ifndef DBCONNECTION_H
#define DBCONNECTION_H
#include <QString>

class dbConnection
{
public:
    QString driver;
    QString adatbazisNev;
    QString server;
    QString user;
    QString password;

    dbConnection(QString, QString, QString, QString, QString);

    QString ToCSV(dbConnection);
    dbConnection FromCSV(QString);
};

#endif // DBCONNECTION_H
