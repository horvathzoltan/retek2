#include "globals.h"
#include "zstringhelper.h"
#include "dbconnection.h"
#include <QStringList>
#include "ztextfilehelper.h"

/*
driver;
adatbazisNev;
server;
user;
password;
*/



dbConnection::dbConnection(QString driver, QString adatbazisNev, QString server, QString user, QString password){
    this->driver = driver;
    this->Name = adatbazisNev;
    this->server = server;
    this->user = user;
    this->password = password;
}


QString dbConnection::ToCSV(){
    QStringList o {driver, Name, server, user, password};
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

QString dbConnection::toCSV() const
{
    return this->driver +zStringHelper::SEP+
    this->Name +zStringHelper::SEP+
    this->server +zStringHelper::SEP+
    this->user +zStringHelper::SEP+
    this->password;
}

QList<dbConnection> dbConnection::fromCSVFile(const QString& fn)
{
    auto txt = zTextFileHelper::load(fn);

    QList<dbConnection> a;
    if(txt.isEmpty()) return a;

    QStringList csvl = zStringHelper::toStringList(txt);

    zforeach(csvr, csvl)
    {
        auto c = FromCSV(*csvr);
        if(c.isValid())
        {
            a << c;
        }
    }

}

bool dbConnection::isValid(){
    return !(
            (Name.isEmpty() || Name.contains(zStringHelper::SEP)) ||
            (server.isEmpty() || server.contains(zStringHelper::SEP))||
            (driver.isEmpty() || driver.contains(zStringHelper::SEP))||
            (password.isEmpty() || password.contains(zStringHelper::SEP))||
            (user.isEmpty() || user.contains(zStringHelper::SEP))
        );
}


bool dbConnection::operator==(dbConnection const& b){
    if(Name==b.Name)
        return true;
    else
        return false;
}

//QString dbConnection::Getname()
//{
//    return this->server+"."+this->Name;
//}
