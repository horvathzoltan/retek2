#ifndef DBCONNECTION_H
#define DBCONNECTION_H
#include <QString>

class dbConnection
{
private:

public:    
    QString driver;
    QString Name;
    QString server;
    QString user;
    QString password;

    dbConnection(QString driver, QString adatbazisNev, QString server, QString user, QString password);

    QString ToCSV();
    static dbConnection FromCSV(QString);

    bool isValid();

    bool operator==(dbConnection const& b);

    //QString Getname();
};


#endif // DBCONNECTION_H
