#ifndef ZSQL_H
#define ZSQL_H

#include <QString>
#include <QSqlDatabase>
#include "ztable.h"
//#include "ztablerow.h"

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
public:
    zSQL();
    void init(QString, QString, QString, QString, QString);

    ~zSQL(void){
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
    }

    void createConnection(QString connectionName="default_conn");

    QString toString();
    QList<QString> getTableNames();
    zTable getTable(QString tablanev);
    QString getTablePK(QString tablanev);
    QString getTable_SQL_PK(QString tablanev, QString cmd);
};

#endif // ZSQL_H
