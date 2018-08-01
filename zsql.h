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

    //QString schemaName;
    QSqlDatabase db;
    QString connectionName;

    QString getLastErrorText();
    QString getConnStr();
    bool createConnection_MSSQL();
    bool createConnection_MYSQL();

    QList<QString> getTableNames_SQL(const QString&);
    zTable getTable_SQL(const QString&, const QString&);

    QString getTable_MSSQL_CMD(const QString& tn);
    QString getTable_MYSQL_CMD(const QString& tn, const QString&);
    QString getTableNames_MYSQL_CMD(const QString&);
    QString getTableNames_MSSQL_CMD();

    QString getTable_MSSQL_PK(const QString& tn);
    QString getTable_MYSQL_PK(const QString& tn);

    bool init(const QString&, const QString&, const QString&, const QString&, const QString&);

public:
    zSQL();
    bool init(dbConnection c);


    ~zSQL(){
        //qDebug("%s", qUtf8Printable( QString("~zSQL %1").arg(connectionName)));

        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }

    bool createConnection(QString connectionName=QStringLiteral("default_conn"));

    QString toString();
    QList<QString> getTableNames(const QString& schemaName);
    zTable getTable(const QString&, const QString&);
    QString getTablePKName(const QString& tablanev);
    QString getTable_SQL_PK(const QString& cmd);

    static const QString getTable_SQL_ENUMTMP;

    QMap<int, QString> getTable_SQL_ENUM(const QString& tablanev, const QString& mezonev);

    /*db elemeinek kezelése*/
    QList<QString> getSchemaNames();
    QList<QString> getSchemaNames_SQL(const QString& cmd);
    static const QString getSchemaNames_MYSQL_CMDTMP;
    static const QString getSchemaNames_MSSQL_CMDTMP;
    QString getSchemaNames_MYSQL_CMD();
    QString getSchemaNames_MSSQL_CMD();
};

#endif // ZSQL_H
