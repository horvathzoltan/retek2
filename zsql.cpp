#include <QTime>
#include <QSqlError>
#include <QSqlQuery>
#include "zsql.h"
#include "globals.h"
#include "zstringmaphelper.h"
#include <QSqlDriver>

// szerver adatokat kitenni izébe, struktúrába
// new zSQL("QMYSQL", "127.0.0.1", "wiki1", "root","Aladar123", "w1"),
const QString zSQL::connectionTemplate = "DRIVER={SQL Server};SERVER=%1;DATABASE=%2;";
const QString zSQL::QODBC = "QODBC";
const QString zSQL::QMYSQL ="QMYSQL";

QString zSQL::getConnStr(){
    return  connectionTemplate.arg(hostName).arg(databaseName);//.arg(driverName);
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
    zlog.log("init:"+this->toString());        
    zlog.log("hasFeature: QuerySize "+QString(db.driver()->hasFeature(QSqlDriver::QuerySize)?"true":"false"));
    }

bool zSQL::createConnection_MSSQL()
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
    db.setDatabaseName(getConnStr());
    db.setUserName(user);
    db.setPassword(password);    
    return db.open();
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
    return db.open();
}

void zSQL::createConnection(QString connectionName){
    connectionName = connectionName;

    bool isok;
    if(driverName == QODBC)
        isok = createConnection_MSSQL();
    else if(driverName == QMYSQL)
        isok = createConnection_MYSQL();
    else{
        zlog.log("createConnection: unknown driver:" + driverName);
        return;
    }

    if(isok)
        zlog.log("Connected");
    else
        zlog.log("Not Connected: " + this->getLastErrorText());

    return;
    }

QList<QString> zSQL::getTableNames(){
    if(db.isValid() && db.isOpen()){
        if(driverName == QODBC)
            return getTableNames_MSSQL();
        else if(driverName == QMYSQL)
            return getTableNames_MYSQL();
        else{
            zlog.log("getTableNames: unknown driver:" + driverName);
        }
    }
    else{
        zlog.log("getTable: db closed" + driverName);
    }
    return QList<QString>();
}

QList<QString> zSQL::getTableNames_MSSQL() {
    QList<QString> eredmeny;

    QString cmd = QString("SELECT "
        "TableName = tbl.table_name, "
        "TableDescription = tableProp.value "
        "FROM information_schema.tables tbl "
        "LEFT JOIN sys.extended_properties tableProp "
        "ON tableProp.major_id = object_id(tbl.table_schema + '.' + tbl.table_name) "
        "AND tableProp.minor_id = 0 "
        "AND tableProp.name = 'MS_Description' "
        "where tbl.table_name not like 'sys%' or tbl.table_name not like '__%'");//.arg(beallitasok.adatbazisNev);

    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);

    while (query.next())
        eredmeny.append(query.value("TableName").toString());

    return eredmeny;
}

QList<QString> zSQL::getTableNames_MYSQL() {
    QList<QString> eredmeny;

    if(db.isValid()){
        auto cmd = QString("SELECT table_name, table_comment "
                           "FROM INFORMATION_SCHEMA.TABLES "
                           "WHERE table_schema='%1';").arg(databaseName);

        if(db.isOpen()){
            QSqlQuery query(cmd, db);
            query.setForwardOnly(true);

                while (query.next())
                    eredmeny.append(query.value("table_name").toString());

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


zTable zSQL::getTable(QString tablanev, QMap<QString, QString> globalCaptionMap, QString fn){

    if(db.isValid() && db.isOpen()){
        if(driverName == QODBC)
            return getTable_MSSQL(tablanev, globalCaptionMap, fn);
        else if(driverName == QMYSQL){}
            //return getTable_MYSQL();
        else{
            zlog.log("getTable: unknown driver:" + driverName);
        }
    }
    else{
        zlog.log("getTable: db closed" + driverName);
    }
    return zTable();
}

zTable zSQL::getTable_MSSQL(QString tablanev, QMap<QString, QString> globalCaptionMap, QString fn)
{
    QString cmd = QString("Select "
        "C.COLUMN_NAME, "
        "C.DATA_TYPE, "
        "C.CHARACTER_MAXIMUM_LENGTH, "
        "C.NUMERIC_PRECISION, "
        "C.NUMERIC_SCALE, "
        "C.IS_NULLABLE, "
        "Case When Z.CONSTRAINT_NAME Is Null Then 0 Else 1 End As IsPartOfPrimaryKey From INFORMATION_SCHEMA.COLUMNS "
        "As C Outer Apply("
        "Select CCU.CONSTRAINT_NAME From INFORMATION_SCHEMA.TABLE_CONSTRAINTS As TC "
        "Join INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE As CCU "
        "On CCU.CONSTRAINT_NAME = TC.CONSTRAINT_NAME "
        "Where TC.TABLE_SCHEMA = C.TABLE_SCHEMA And TC.TABLE_NAME = C.TABLE_NAME And TC.CONSTRAINT_TYPE = 'PRIMARY KEY' And CCU.COLUMN_NAME = C.COLUMN_NAME) As Z "
        "Where C.TABLE_NAME = '%1'").arg(tablanev);

    QSqlQuery query(cmd, db);

    QList<zTablerow> tr;

    QMap<QString, QString> tablaCaptionMap;
    zStringMapHelper::StringMapFeltolt(fn, &tablaCaptionMap);

    while (query.next()) {
        QString colName = query.value("COLUMN_NAME").toString();
        QString dtype = query.value("DATA_TYPE").toString();
        int dlen = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();
        bool nullable = query.value("IS_NULLABLE").toBool();
        QString cn = colName.toLower();

        QString caption = tablaCaptionMap.contains(cn)?tablaCaptionMap[cn]:globalCaptionMap.contains(cn)?globalCaptionMap[cn]:cn;

        tr.append(zTablerow(colName, dtype, dlen, nullable, caption));
        }

    auto e = zTable(tablanev, tr);
    return e;
}
