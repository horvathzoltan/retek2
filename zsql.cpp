#include <QTime>
#include <QSqlError>
#include <QSqlQuery>
#include "zsql.h"
#include "globals.h"
#include "zstringmaphelper.h"
#include <QSqlDriver>
#include "zstringhelper.h"

// szerver adatokat kitenni izébe, struktúrába
// new zSQL("QMYSQL", "127.0.0.1", "wiki1", "root","Aladar123", "w1"),
const QString zSQL::connectionTemplate = "DRIVER={SQL Server};SERVER=%1;DATABASE=%2;";
const QString zSQL::QODBC = "QODBC";
const QString zSQL::QMYSQL ="QMYSQL";

QString zSQL::getConnStr(){
    return  connectionTemplate.arg(hostName).arg(databaseName);//.arg(driverName);
}

zSQL::zSQL(){};

bool zSQL::init(dbConnection c){
    return init(c.driver, c.server, c.adatbazisNev, c.user, c.password);
}

bool zSQL::init(QString _driverName, QString _hostName, QString _databaseName, QString _user, QString _pass){
    connectionName = "conn1";
    user = _user;
    password = _pass;
    driverName = _driverName;
    hostName = _hostName;
    databaseName = _databaseName;    
    bool isok = this->createConnection();
    if(isok){
        zlog.log("init:"+this->toString());
        zlog.log("hasFeature: QuerySize "+QString(db.driver()->hasFeature(QSqlDriver::QuerySize)?"true":"false"));
        }
    return isok;
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

bool zSQL::createConnection(QString connectionName){
    connectionName = connectionName;

    bool isok;
    if(driverName == QODBC)
        isok = createConnection_MSSQL();
    else if(driverName == QMYSQL)
        isok = createConnection_MYSQL();
    else{
        zlog.log("createConnection: unknown driver:" + driverName);
        return false;
    }

    if(isok)
        zlog.log("Connected");
    else
        zlog.log("Not Connected: " + this->getLastErrorText());

    return isok;
    }

const QString zSQL::getTableNames_MYSQL_CMDTMP = "SELECT table_name AS TableName, table_comment AS TableDescription "
                                                 "FROM INFORMATION_SCHEMA.TABLES "
                                                 "WHERE table_schema='%1';";

const QString zSQL::getTableNames_MSSQL_CMDTMP = "SELECT "
                                                 "TableName = tbl.table_name, "
                                                 "TableDescription = tableProp.value "
                                                 "FROM information_schema.tables tbl "
                                                 "LEFT JOIN sys.extended_properties tableProp "
                                                 "ON tableProp.major_id = object_id(tbl.table_schema + '.' + tbl.table_name) "
                                                 "AND tableProp.minor_id = 0 "
                                                 "AND tableProp.name = 'MS_Description' "
                                                 "where tbl.table_name not like 'sys%' or tbl.table_name not like '__%'";

QString zSQL::getTableNames_MSSQL_CMD(){ return getTableNames_MSSQL_CMDTMP; }
QString zSQL::getTableNames_MYSQL_CMD(){ return getTableNames_MYSQL_CMDTMP.arg(this->databaseName); }

QList<QString> zSQL::getTableNames(){
    if(db.isValid() && db.isOpen()){
        if(driverName == QODBC)
            return getTableNames_SQL(getTableNames_MSSQL_CMD());//.arg(beallitasok.adatbazisNev);
        else if(driverName == QMYSQL)
            return getTableNames_SQL(getTableNames_MYSQL_CMD());
        else{
            zlog.log("getTableNames: unknown driver:" + driverName);
        }
    }
    else{
        zlog.log("getTable: db closed" + driverName);
    }
    return QList<QString>();
}


QList<QString> zSQL::getTableNames_SQL(QString cmd) {
    QList<QString> eredmeny;

    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);

    while (query.next())
        eredmeny.append(query.value("TableName").toString());
    return eredmeny;
}

QString zSQL::getLastErrorText(){
    return db.lastError().text();
}

QString zSQL::toString()
{
    return this->databaseName+":"+this->connectionName;
}

const QString zSQL::getTable_MYSQL_CMDTMP ="SELECT "
                                           "COLUMN_NAME, "
                                           "DATA_TYPE, "
                                           "CHARACTER_MAXIMUM_LENGTH, "
                                           "NUMERIC_PRECISION, "
                                           "NUMERIC_SCALE, "
                                           "IS_NULLABLE "
                                           "FROM INFORMATION_SCHEMA.COLUMNS "
                                           "where table_schema = '%1' and table_name = '%2';";

const QString zSQL::getTable_MSSQL_CMDTMP = "Select "
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
                                            "Where C.TABLE_NAME = '%1'";

QString zSQL::getTable_MSSQL_CMD(QString tn){ return getTable_MSSQL_CMDTMP.arg(tn); }
QString zSQL::getTable_MYSQL_CMD(QString tn){ return getTable_MYSQL_CMDTMP.arg(this->databaseName).arg(tn); }


zTable zSQL::getTable(const QString& tablanev){

    if(db.isValid() && db.isOpen()){
        QString fn = beallitasok.getCaptionFileName(tablanev);

        if(driverName == QODBC){
            return getTable_SQL(tablanev, fn, getTable_MSSQL_CMD(tablanev));
        }
        else if(driverName == QMYSQL){
            return getTable_SQL(tablanev, fn, getTable_MYSQL_CMD(tablanev));
        }
        else{
            zlog.log("getTable: unknown driver:" + driverName);
        }
    }
    else{
        zlog.log("getTable: db closed" + driverName);
    }
    return zTable();
}

zTable zSQL::getTable_SQL(QString tablanev, QString fn, QString cmd)
{
    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);

    QList<zTablerow> tr;

    QMap<QString, QString> tablaCaptionMap;
    zStringMapHelper::StringMapFeltolt(fn, &tablaCaptionMap);

    while (query.next()) {
        QString colName = query.value("COLUMN_NAME").toString();
        QString dtype = query.value("DATA_TYPE").toString();
        int dlen = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();

        bool nullable = zStringHelper::toBool(query.value("IS_NULLABLE").toString());
        QString cn = colName.toLower();

        QString caption = tablaCaptionMap.contains(cn)?tablaCaptionMap[cn]:globalCaptionMap.contains(cn)?globalCaptionMap[cn]:cn;

        tr.append(zTablerow(colName, dtype, dlen, nullable, caption));
        }

    QString pkn = getTablePKName(tablanev);

    //QList<zTablerow> pr;

    auto e = zTable(nullptr, (pkn.isEmpty()?"zId":pkn), tr, SQL, tablanev);
    return e;
}


/*
PK kezelés
*/

const QString zSQL::getTable_MSSQL_PKTMP = "SELECT COLUMN_NAME "
                                           "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE "
                                           "WHERE OBJECTPROPERTY(OBJECT_ID(CONSTRAINT_SCHEMA + '.' + QUOTENAME(CONSTRAINT_NAME)), 'IsPrimaryKey') = 1 "
                                           "AND TABLE_NAME = '%1';";

// AND TABLE_CATALOG ='%2' AND TABLE_SCHEMA = 'dbo';";// AND TABLE_CATALOG='enyv';";

const QString zSQL::getTable_MYSQL_PKTMP = "SELECT COLUMN_NAME "
                                           "FROM INFORMATION_SCHEMA.COLUMNS "

                                           "where table_name = '%1' "
                                           "and COLUMN_KEY = 'PRI';";
//   "where table_schema = '%1' "

QString zSQL::getTable_MSSQL_PK(QString tn){
    return getTable_MSSQL_PKTMP.arg(tn);//.arg(this->databaseName);
}

QString zSQL::getTable_MYSQL_PK(QString tn){
    return getTable_MYSQL_PKTMP.arg(tn);//.arg(this->databaseName);
}

QString zSQL::getTablePKName(QString tablanev){

    if(db.isValid() && db.isOpen()){
        if(driverName == QODBC)
            return getTable_SQL_PK(getTable_MSSQL_PK(tablanev));
        else if(driverName == QMYSQL)
            return getTable_SQL_PK(getTable_MYSQL_PK(tablanev));
        else
            zlog.log("getTable: unknown driver:" + driverName);        
    }
    else
        zlog.log("getTable: db closed" + driverName);    
    return "";
}

QString zSQL::getTable_SQL_PK(QString cmd)
{
    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);

    if(query.next()) {
        return query.value("COLUMN_NAME").toString();
        }

    return "";
}


const QString zSQL::getTable_SQL_ENUMTMP = "SELECT id, %1 as name FROM %2";

QMap<int, QString> zSQL::getTable_SQL_ENUM(QString tablanev, QString mezonev)
{
    QString cmd = getTable_SQL_ENUMTMP.arg(mezonev).arg(tablanev);

    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);
    auto e = QMap<int, QString>();

    while(query.next()) {
        auto i = query.value("id").toInt();
        auto n = query.value("name").toString();
        e.insert(i, n);
        }

    return e;
}

