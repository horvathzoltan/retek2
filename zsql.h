#ifndef ZSQL_H
#define ZSQL_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include "ztable.h"
#include "dbconnection.h"

class zSQL
{
private:    
    static const QString connectionTemplate;
    static const QString QODBC;
    static const QString QMYSQL;

    static const QString getTableNames_MYSQL_CMDTMP;
    static const QString getTableNames_MSSQL_CMDTMP;
    /*
A tábla PK mező nevének lekérdezése
*/
    static const QString getTable_MSSQL_PKTMP;
    static const QString getTable_MYSQL_PKTMP;

    static const QString getTable_MYSQL_CMDTMP;
    static const QString getTable_MSSQL_CMDTMP;

    QString driverName,
            hostName,
            user,
            password;

    QString databaseName;
    QSqlDatabase db;
    QString connectionName;

    QString getLastErrorText();
    QString getConnStr();
    bool createConnection_MSSQL();
    bool createConnection_MYSQL();

    QList<QString> getTableNames_SQL(QString);
    zTable getTable_SQL(QString tablanev, QString fn, QString);

    QString getTable_MSSQL_CMD(QString tn);
    QString getTable_MYSQL_CMD(QString tn);
    QString getTableNames_MYSQL_CMD();
    QString getTableNames_MSSQL_CMD();

    QString getTable_MSSQL_PK(QString tn);
    QString getTable_MYSQL_PK(QString tn);

    bool init(QString, QString, QString, QString, QString);

public:
    zSQL();
    bool init(dbConnection c);


    ~zSQL(void){        
        //qDebug("%s", qUtf8Printable( QString("~zSQL %1").arg(connectionName)));

        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);

        return;
    }

    bool createConnection(QString connectionName="default_conn");

    QString toString();
    QList<QString> getTableNames();
    zTable getTable(QString tablanev);
    QString getTablePKName(QString tablanev);
    QString getTable_SQL_PK(QString cmd);

    static const QString getTable_SQL_ENUMTMP;

    QMap<int, QString> getTable_SQL_ENUM(QString tablanev, QString mezonev);

};

#endif // ZSQL_H
