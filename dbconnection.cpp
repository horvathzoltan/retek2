#include "dbconnection.h"
#include <QStringList>

/*
driver;
adatbazisNev;
server;
user;
password;
*/
dbConnection::dbConnection(QString driver, QString adatbazisNev, QString server, QString user, QString password){
    this->driver = driver;
    this->adatbazisNev = adatbazisNev;
    this->server = server;
    this->user = user;
    this->password = password;
}


QString dbConnection::ToCSV(dbConnection i){
    QStringList o {i.driver, i.adatbazisNev, i.server, i.user, i.password};
    return o.join(';');
}

dbConnection dbConnection::FromCSV(QString i){
    QStringList a = i.split(';');
    if(a.count()==5){
        return dbConnection(a[0],a[1],a[2],a[3],a[4]);
    }
    else
        return dbConnection("","","","","");
}
