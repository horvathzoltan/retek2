#include "zstringHelper.h"
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


QString dbConnection::ToCSV(){
    QStringList o {driver, adatbazisNev, server, user, password};
    return o.join(zStringHelper::SEP);
}

dbConnection dbConnection::FromCSV(QString i){
    QStringList a = i.split(zStringHelper::SEP);
    if(a.count()==5){
        return dbConnection(a[0],a[1],a[2],a[3],a[4]);
    }
    else
        return dbConnection("","","","","");
}


bool dbConnection::isValid(){
    return !(
            (adatbazisNev.isEmpty() || adatbazisNev.contains(zStringHelper::SEP)) ||
            (server.isEmpty() || server.contains(zStringHelper::SEP))||
            (driver.isEmpty() || driver.contains(zStringHelper::SEP))||
            (password.isEmpty() || password.contains(zStringHelper::SEP))||
            (user.isEmpty() || user.contains(zStringHelper::SEP))
        );
}


bool dbConnection::operator==(dbConnection const& b){
    if(adatbazisNev==b.adatbazisNev)
        return true;
    else
        return false;
}

QString dbConnection::Getname()
{
    return this->server+"."+this->adatbazisNev;
}
