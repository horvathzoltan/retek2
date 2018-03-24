#include "dbconnection.h"
#include <QStringList>

/*
driver;
adatbazisNev;
server;
user;
password;
*/

const QChar dbConnection::SEP = ';';

dbConnection::dbConnection(QString driver, QString adatbazisNev, QString server, QString user, QString password){
    this->driver = driver;
    this->adatbazisNev = adatbazisNev;
    this->server = server;
    this->user = user;
    this->password = password;
}


QString dbConnection::ToCSV(){
    QStringList o {driver, adatbazisNev, server, user, password};
    return o.join(SEP);
}

dbConnection dbConnection::FromCSV(QString i){
    QStringList a = i.split(SEP);
    if(a.count()==5){
        return dbConnection(a[0],a[1],a[2],a[3],a[4]);
    }
    else
        return dbConnection("","","","","");
}


bool dbConnection::isValid(){
    return !(
            (adatbazisNev.isEmpty() || adatbazisNev.contains(SEP)) ||
            (server.isEmpty() || server.contains(SEP))||
            (driver.isEmpty() || driver.contains(SEP))||
            (password.isEmpty() || password.contains(SEP))||
            (user.isEmpty() || user.contains(SEP))
        );
}


bool dbConnection::operator==(dbConnection const& b){
    if(adatbazisNev==b.adatbazisNev)
        return true;
}

QString dbConnection::Getname()
{
    return this->server+"."+this->adatbazisNev;
}
