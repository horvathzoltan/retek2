#include <QTime>
#include <QSqlError>
#include <QSqlQuery>
#include "zsql.h"
#include "globals.h"

// szerver adatokat kitenni izébe, struktúrába
// new zSQL("QMYSQL", "127.0.0.1", "wiki1", "root","Aladar123", "w1"),
const QString zSQL::connectionTemplate = "DRIVER={SQL Server};SERVER=%1;DATABASE=%2;";
const QString zSQL::QODBC = "QODBC";
const QString zSQL::QMYSQL ="QMYSQL";

QString zSQL::getConnStr(){
    return  connectionTemplate.arg(hostName).arg(databaseName).arg(driverName);
}

zSQL::zSQL(){};

void zSQL::init(QString _driverName, QString _hostName, QString _databaseName, QString _user, QString _pass){
    connectionName = "conn1";
    user = _user;
    password = _pass;
    driverName = _driverName;
    hostName = _hostName;
    databaseName = _databaseName;    
    this->createConnection();
    zlog.log(this->toString());
    }

bool zSQL::createConnection_MSSQL()
{    
    db = QSqlDatabase::addDatabase(driverName, connectionName);
    db.setHostName(hostName);
    db.setDatabaseName(getConnStr());
    db.setUserName(user);
    db.setPassword(password);    
    return db.isValid();
}

bool zSQL::createConnection_MYSQL()
{        
    if(db.isValid()){
        if(db.isOpen()){
            db.close();
        }
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }
    db = QSqlDatabase::addDatabase(driverName, connectionName);
    db.setHostName(hostName);
    db.setDatabaseName(databaseName);
    db.setUserName(user);
    db.setPassword(password);
    db.setConnectOptions("MYSQL_OPT_CONNECT_TIMEOUT=10");
    return db.isValid();
}

void zSQL::createConnection(QString connectionName){
    connectionName = connectionName;

    bool isok;
    if(driverName == QODBC)
        isok = createConnection_MSSQL();
    else if(driverName == QMYSQL)
        isok = createConnection_MYSQL();
    else{
        zlog.log("cannot establish connection with driver:" + driverName);
        return;
    }

    if(isok)
        zlog.log("Connected");
    else
        zlog.log("Not Connected: " + this->getLastErrorText());

    return;
    }

QList<QString> zSQL::getTableNames(){

    if(driverName == QODBC)
        return getTableNames_MSSQL();
    else if(driverName == QMYSQL)
        return getTableNames_MYSQL();
    else{
        zlog.log("cannot get tablenames with driver:" + driverName);
        return QList<QString>();
    }
}

QList<QString> zSQL::getTableNames_MSSQL() {
    /*

    QString commandText = "SELECT "
        "TableName = tbl.table_name, "
        "TableDescription = tableProp.value "
        "FROM information_schema.tables tbl "
        "LEFT JOIN sys.extended_properties tableProp "
        "ON tableProp.major_id = object_id(tbl.table_schema + '.' + tbl.table_name) "
        "AND tableProp.minor_id = 0 "
        "AND tableProp.name = 'MS_Description' "
        "where tbl.table_name not like 'sys%' or tbl.table_name not like '__%' order by tbl.table_name asc;";

    QString commandTextTemplate2 ="select table_name as TableName from information_schema.tables where TABLE_SCHEMA='%1'";
    QString commandText2 = commandTextTemplate2.arg(beallitasok.adatbazisNev);

    QSqlQuery query(commandText2);

    ui.listWidget_tabla->clear();

    while (query.next()) {
        QString tablename = query.value("TableName").toString();

        new QListWidgetItem(tablename, ui.listWidget_tabla);
    }
    */
}

QList<QString> zSQL::getTableNames_MYSQL() {
    QList<QString> eredmeny;

    if(db.isValid()){
        auto cmd = QString("SELECT table_name, table_comment "
                           "FROM INFORMATION_SCHEMA.TABLES "
                           "WHERE table_schema='%1';").arg(databaseName);
        db.open();
        if(db.isOpen()){
            QSqlQuery query(cmd, db);
            query.setForwardOnly(true);
            if(query.exec()){
                while (query.next())
                    eredmeny.append(query.value("table_name").toString());
                }
            db.close();
            }
        }
    return eredmeny;
}

QString zSQL::getLastErrorText(){
    return db.lastError().text();
}

QString zSQL::toString()
{
    return this->databaseName+":"+this->connectionName;
}

