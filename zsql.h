#ifndef ZSQL_H
#define ZSQL_H

#include <QString>
#include <QSqlDatabase>

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
public:
    QString databaseName;
    QSqlDatabase db;
    QString connectionName;

    zSQL();
    zSQL(QString, QString, QString, QString, QString);

    ~zSQL(void){
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
    }

    void createConnection(QString connectionName="default_conn");
    QString getLastErrorText();
    //QList<zField> getFields(QString tablename);
    QString toString();
    QString getConnStr();
};

#endif // ZSQL_H
