#include <QTime>
#include <QSqlError>
#include "zsql.h"
#include "globals.h"

// szerver adatokat kitenni izébe, struktúrába
// new zSQL("QMYSQL", "127.0.0.1", "wiki1", "root","Aladar123", "w1"),
const QString zSQL::connectionTemplate = "DRIVER={SQL Server};SERVER=%1;DATABASE=%2;";
const QString zSQL::QODBC = "QODBC";
const QString zSQL::QMYSQL ="QMYSQL";

QString zSQL::getConnStr(){
    return  connectionTemplate.arg(hostName).arg(databaseName).arg(driverName);
}

zSQL::zSQL(){};

zSQL::zSQL(QString _driverName, QString _hostName, QString _databaseName, QString _user, QString _pass){
    connectionName = "";
    user = _user;
    password = _pass;
    driverName = _driverName;
    hostName = _hostName;
    databaseName = _databaseName;
    this->createConnection();
    zlog.log(this->toString());
    }

void zSQL::createConnection(QString connectionName){
    connectionName = connectionName;
    QString connstr = getConnStr();

    db = QSqlDatabase::addDatabase(driverName, connectionName);
    db.setHostName(hostName);
    db.setDatabaseName(connstr);
    db.setUserName(user);
    db.setPassword(password);
    //db.setConnectOptions("MYSQL_OPT_CONNECT_TIMEOUT=10");

    auto isOK = db.open();

    if(!isOK){
        zlog.log("Not Connected: " + this->getLastErrorText());
        }
#ifdef QT_DEBUG
    else{
        zlog.log("Connected");
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

