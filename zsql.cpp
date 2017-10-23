#include <QTime>
#include <QSqlError>
#include "zsql.h"
#include "globals.h"

// szerver adatokat kitenni izébe, struktúrába
// new zSQL("QMYSQL", "127.0.0.1", "wiki1", "root","Aladar123", "w1"),
const QString zSQL::connectionTemplate = "DRIVER={%3};SERVER=%1;DATABASE=%2;";

QString zSQL::getConnStr(QString driverName, QString serverName, QString dbName){
    return  connectionTemplate.arg(serverName).arg(dbName).arg(driverName);
}

zSQL::zSQL(QString _driverName, QString _hostName, QString _databaseName, QString _user, QString _pass, QString _name){
    connectionName = _name;
    user = _user;
    password = _pass;
    driverName = _driverName;
    hostName = _hostName;
    databaseName = _databaseName;
    this->createConnection();
    zlog.log(this->toString());
    }

void zSQL::createConnection(){
    //db = new QSqlDatabase(); //http://www.qtcentre.org/threads/45208-QSqlDatabase-best-practices-with-long-running-application
    db = QSqlDatabase::addDatabase(driverName, connectionName);
    db.setHostName(hostName);
    db.setDatabaseName(databaseName);
    db.setUserName(user);
    db.setPassword(password);
    db.setConnectOptions("MYSQL_OPT_CONNECT_TIMEOUT=10");

    auto isOK = db.open();

    if(!isOK){
        zlog.log("DBERR: " + this->getLastErrorText());
        }
#ifdef QT_DEBUG
    else{
        zlog.log("DBOK");
        }
#endif

    return;
    }



QString zSQL::getLastErrorText(){
    return db.lastError().text();
}

QString zSQL::toString()
{
    return this->databaseName+":"+this->connectionName;
}

