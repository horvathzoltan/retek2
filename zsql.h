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

    QString driverName,
            hostName,
            user,
            password;

    QString databaseName;
    QSqlDatabase db;
    QString connectionName;

public:
    zSQL();
    void init(QString, QString, QString, QString, QString);

    ~zSQL(void){
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
    }

    void createConnection(QString connectionName="default_conn");

    //QList<zField> getFields(QString tablename);
    QString toString();


    QList<QString> getTableNames();
    zTable zSQL::getTable(QString tablanev, QMap<QString, QString> globalCaptionMap, QString fn);

protected:
    QString getLastErrorText();
    QString getConnStr();
    bool createConnection_MSSQL();
    bool createConnection_MYSQL();

    QList<QString> getTableNames_MYSQL();
    QList<QString> getTableNames_MSSQL();

    zTable zSQL::getTable_MSSQL(QString tablanev, QMap<QString, QString> globalCaptionMap, QString fn);
    int size(QSqlQuery);
};

#endif // ZSQL_H
