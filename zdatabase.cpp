#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

#include "zdatabase.h"
#include "globals.h"

const QString zDataBase::connectionTemplate = "DRIVER={%3};SERVER=%1;DATABASE=%2;";

zDataBase::zDataBase()
{

}

//zSQL("QMYSQL", "127.0.0.1", "wiki1", "root","Aladar123", "w1"),

bool zDataBase::Connect(QString driver, QString hostname, QString databasename,  QString u, QString p)
{
    QString connstr = getConnStr(driver, hostname, databasename);

    auto db = QSqlDatabase::addDatabase(driver);


    db.setHostName(hostname);
    db.setDatabaseName(databasename);

    //db.setDatabaseName(connstr);
    db.setUserName(u);
    db.setPassword(p);

    bool is_dbOK = db.open();

    if (!is_dbOK) {
        QSqlError err = db.lastError();
        zlog.log(QString("Connect: %1").arg(err.text()));
    }
    else{
        zlog.log(QString("Connect: %1").arg(connstr));
    }

   return is_dbOK;
}

QString zDataBase::getConnStr(QString driverName, QString serverName, QString dbName){
    return  connectionTemplate.arg(serverName).arg(dbName).arg(driverName);
}
