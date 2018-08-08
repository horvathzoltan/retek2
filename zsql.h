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

    QString connectionName,
            driverName,
            hostName,
            user,
            password;

    QSqlDatabase db;

    QString getLastErrorText();
    QString getConnStr();
    bool createConnection_MSSQL();
    bool createConnection_MYSQL();

    /*schema*/
    static const QString getSchemaNames_MYSQL_CMDTMP;
    static const QString getSchemaNames_MSSQL_CMDTMP;

    QString getSchemaNames_MYSQL_CMD();
    QString getSchemaNames_MSSQL_CMD();

    QStringList getSchemaNames_SQL(const QString& cmd);

    /*table*/
    static const QString getTable_MYSQL_CMDTMP;
    static const QString getTable_MSSQL_CMDTMP;

    QString getTable_MSSQL_CMD(const QString& schemaName, const QString& tn);
    QString getTable_MYSQL_CMD(const QString&, const QString&);
    zTable getTable_SQL(const QString&, const QString&, const QStringList &fl = QStringList());

    /*tableNames*/
    static const QString getTableNames_MYSQL_CMDTMP;
    static const QString getTableNames_MSSQL_CMDTMP;

    QString getTableNames_MYSQL_CMD(const QString&);
    QString getTableNames_MSSQL_CMD(const QString&);
    QStringList getTableNames_SQL(const QString&);

    /*fieldNames*/
    static const QString getFieldNames_MYSQL_CMDTMP;
    static const QString getFieldNames_MSSQL_CMDTMP;

    QString getFieldNames_MSSQL_CMD(const QString& schemaName, const QString& tn);
    QString getFieldNames_MYSQL_CMD(const QString& schemaName, const QString& tn);
    QStringList getFieldNames_SQL(const QString&);

    /*PKName*/
    static const QString getTable_MSSQL_PKTMP;
    static const QString getTable_MYSQL_PKTMP;

    QString getTable_MSSQL_PK(const QString& tn);
    QString getTable_MYSQL_PK(const QString& tn);
    QString getTable_SQL_PK(const QString& cmd);

    bool init(const QString&, const QString&, const QString&, const QString&, const QString&);

public:
    zSQL();
    bool init(const dbConnection &c);

    ~zSQL(){
        //qDebug("%s", qUtf8Printable( QString("~zSQL %1").arg(connectionName)));

        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }

    bool createConnection(QString connectionName=QStringLiteral("default_conn"));

    QString toString();

    zTable getTable(const QString&, const QString&, const QStringList &fl = QStringList());

    static const QString getTable_SQL_ENUMTMP;

    QMap<int, QString> getTable_SQL_ENUM(const QString& tablanev, const QString& mezonev);

    /*db elemeinek kezelése*/
    QStringList getSchemaNames();
    QStringList getTableNames(const QString& schemaName);
    QStringList getFieldNames(const QString& schemaName, const QString& tableName);
    QString getTablePKName(const QString& tablanev);
};

#endif // ZSQL_H
