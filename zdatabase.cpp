#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

#include "zdatabase.h"
#include "Beallitasok.h"

zDataBase::zDataBase()
{

}

bool zDataBase::Connect(QString connectionString, QString u, QString p)
{
    //Beallitasok::get();
    //QString connectionString =  Beallitasok::getConnStr();

    auto db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName(connectionString);
    db.setUserName(u);
    db.setPassword(p);

    bool is_dbOK = db.open();

    if (!is_dbOK) {
        QSqlError err = db.lastError();
        qDebug() << "QSqlError: " << err;
    }

   return is_dbOK;
}
