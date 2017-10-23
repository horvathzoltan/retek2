#ifndef ZSQL_H
#define ZSQL_H

#include <QString>
#include <QSqlDatabase>

class zSQL
{
private:
    static const QString connectionTemplate;

    QString driverName,
            hostName,
            user,
            password;
public:
    QString databaseName;
    QSqlDatabase db;
    QString connectionName;
public:
    zSQL(QString, QString, QString, QString, QString, QString);

    ~zSQL(void){
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
    }

    void createConnection();
    QString getLastErrorText();
    //QList<zField> getFields(QString tablename);
    QString toString();
    static QString getConnStr(QString driverName, QString serverName, QString dbName);
};

#endif // ZSQL_H
