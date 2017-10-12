#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

#include "zdatabase.h"
#include "globals.h"

zDataBase::zDataBase()
{

}

bool zDataBase::Connect(QString connectionString, QString u, QString p)
{
    auto db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName(connectionString);
    db.setUserName(u);
    db.setPassword(p);

    bool is_dbOK = db.open();

    if (!is_dbOK) {
        QSqlError err = db.lastError();
        zlog.log(QString("QSqlError: %1").arg(err.text()));
    }
    else{
        zlog.log(QString("QSql: %1").arg(connectionString));
    }

   return is_dbOK;
}
