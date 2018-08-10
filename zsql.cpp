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
//const QString zSQL::connectionTemplate = "DRIVER={SQL Server};SERVER=%1;DATABASE=%2;";
const QString zSQL::connectionTemplate = QStringLiteral("DRIVER={SQL Server};SERVER=%1;");
const QString zSQL::QODBC = QStringLiteral("QODBC");
const QString zSQL::QMYSQL = QStringLiteral("QMYSQL");

QString zSQL::getConnStr(){
    //return  connectionTemplate.arg(hostName).arg(schemaName);//.arg(driverName);
    return  connectionTemplate.arg(hostName);//.arg(driverName);
}

zSQL::zSQL() = default;

bool zSQL::init(const dbConnection& c){
    return init(c.driver, c.server, c.Name, c.user, c.password);
}

bool zSQL::init(const QString &_driverName, const QString& _hostName, const QString& _connectionName, const QString& _user, const QString& _pass){
    connectionName = QStringLiteral("conn1");
    user = _user;
    password = _pass;
    driverName = _driverName;
    hostName = _hostName;
    //schemaName = _databaseName;
    connectionName = _connectionName;
    bool isok = this->createConnection();
    if(isok){
        zlog.trace("init:"+this->toString());
        zlog.trace(QStringLiteral("hasFeature: QuerySize: %1").arg(db.driver()->hasFeature(QSqlDriver::QuerySize)?QStringLiteral("true"):QStringLiteral("false")));
        }
    return isok;
    }

bool zSQL::createConnection_MSSQL()
{    
    if(db.isValid())
    {
        if(db.isOpen())
        {
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
    if(db.isValid())
    {
        if(db.isOpen())
        {
            db.close();
        }
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }
    db = QSqlDatabase::addDatabase(driverName, connectionName);
    db.setHostName(hostName);
    //db.setDatabaseName(schemaName);
    db.setUserName(user);
    db.setPassword(password);
    db.setConnectOptions(QStringLiteral("MYSQL_OPT_CONNECT_TIMEOUT=10"));
    return db.open();
}

bool zSQL::createConnection(QString connectionName){
    connectionName = connectionName;

    bool isok;
    if(driverName == QODBC)
    {
        isok = createConnection_MSSQL();
    }
    else if(driverName == QMYSQL)
    {
        isok = createConnection_MYSQL();
    }
    else
    {
        zlog.error("createConnection: unknown driver:" + driverName);
        return false;
    }

    if(isok)        
    {
        zlog.ok("Connected");
    }
    else
    {
        zlog.error("Not Connected: " + this->getLastErrorText());
    }

    return isok;
    }

/*table*/
const QString zSQL::getTableNames_MYSQL_CMDTMP = "SELECT table_name AS TableName, table_comment AS TableDescription "
                                                 "FROM INFORMATION_SCHEMA.TABLES "
                                                 "WHERE table_schema='%1' ORDER BY TableName;";

const QString zSQL::getTableNames_MSSQL_CMDTMP = "SELECT "
                                                 "TableName = tbl.table_name, "
                                                 "TableDescription = tableProp.value "
                                                 "FROM [%1].information_schema.tables tbl "
                                                 "LEFT JOIN sys.extended_properties tableProp "
                                                 "ON tableProp.major_id = object_id(tbl.table_schema + '.' + tbl.table_name) "
                                                 "AND tableProp.minor_id = 0 "
                                                 "AND tableProp.name = 'MS_Description' "
                                                 "where tbl.table_name not like 'sys%' or tbl.table_name not like '__%' ORDER BY TableName";

QString zSQL::getTableNames_MSSQL_CMD(const QString& schemaName){ return getTableNames_MSSQL_CMDTMP.arg(schemaName); }
QString zSQL::getTableNames_MYSQL_CMD(const QString& schemaName){ return getTableNames_MYSQL_CMDTMP.arg(schemaName); }

QStringList zSQL::getTableNames(const QString& schemaName){
    if(db.isValid() && db.isOpen()){
        if(driverName == QODBC)
        {
            return getTableNames_SQL(getTableNames_MSSQL_CMD(schemaName));//.arg(beallitasok.adatbazisNev);
        }
        if(driverName == QMYSQL)
        {
            return getTableNames_SQL(getTableNames_MYSQL_CMD(schemaName));
        }        
        zlog.error("getTableNames: unknown driver:" + driverName);
    }
    else
    {
        zlog.error("getTable: db closed" + driverName);
    }
    return QStringList();
}


QStringList zSQL::getTableNames_SQL(const QString& cmd) {
    QStringList eredmeny;

    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);

    while (query.next())
    {
        eredmeny.append(query.value(QStringLiteral("TableName")).toString());
    }
    return eredmeny;
}

/* fieldNames */
// MSSQL_CMDTMP - SELECT... FROM [%1]...
// https://www.mssqltips.com/sqlservertip/1781/list-columns-and-attributes-for-every-table-in-a-sql-server-database/
const QString zSQL::getFieldNames_MYSQL_CMDTMP ="SELECT "
                                           "COLUMN_NAME as fieldName "
                                           "FROM INFORMATION_SCHEMA.COLUMNS "
                                           "where table_schema = '%1' and table_name = '%2';";

const QString zSQL::getFieldNames_MSSQL_CMDTMP = "SELECT "
                                            "COLUMN_NAME as fieldName "
                                            "FROM [%1].INFORMATION_SCHEMA.COLUMNS "
                                            "WHERE TABLE_NAME = '%2'";

QString zSQL::getFieldNames_MSSQL_CMD(const QString& schemaName, const QString& tn){ return getFieldNames_MSSQL_CMDTMP.arg(schemaName, tn); }
QString zSQL::getFieldNames_MYSQL_CMD(const QString& schemaName, const QString& tn){ return getFieldNames_MYSQL_CMDTMP.arg(schemaName, tn); }

QStringList zSQL::getFieldNames(const QString& schemaName, const QString& tableName){
    if(db.isValid() && db.isOpen()){
        if(driverName == QODBC)
        {
            return getFieldNames_SQL(getFieldNames_MSSQL_CMD(schemaName, tableName));//.arg(beallitasok.adatbazisNev);
        }
        if(driverName == QMYSQL)
        {
            return getFieldNames_SQL(getFieldNames_MYSQL_CMD(schemaName, tableName));
        }
        zlog.error("getFieldNames: unknown driver:" + driverName);
    }
    else
    {
        zlog.error("getField: db closed" + driverName);
    }
    return QStringList();
}

QStringList zSQL::getFieldNames_SQL(const QString& cmd) {
    QStringList eredmeny;

    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);

    while (query.next())
    {
        eredmeny.append(query.value(QStringLiteral("fieldName")).toString());
    }
    return eredmeny;
}

QString zSQL::getLastErrorText(){
    return db.lastError().text();
}

QString zSQL::toString()
{
    //return this->schemaName+":"+this->connectionName;
    return this->connectionName;
}

/*tableNames */

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
                                            "Case When Z.CONSTRAINT_NAME Is Null Then 0 Else 1 End As IsPartOfPrimaryKey From [%1].INFORMATION_SCHEMA.COLUMNS "
                                            "As C Outer Apply("
                                            "Select CCU.CONSTRAINT_NAME From [%1].INFORMATION_SCHEMA.TABLE_CONSTRAINTS As TC "
                                            "Join INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE As CCU "
                                            "On CCU.CONSTRAINT_NAME = TC.CONSTRAINT_NAME "
                                            "Where TC.TABLE_SCHEMA = C.TABLE_SCHEMA And TC.TABLE_NAME = C.TABLE_NAME And TC.CONSTRAINT_TYPE = 'PRIMARY KEY' And CCU.COLUMN_NAME = C.COLUMN_NAME) As Z "
                                            "Where C.TABLE_NAME = '%2'";

QString zSQL::getTable_MSSQL_CMD(const QString& schemaName, const QString& tn){ return getTable_MSSQL_CMDTMP.arg(schemaName, tn); }
QString zSQL::getTable_MYSQL_CMD(const QString& schemaName, const QString& tn ){ return getTable_MYSQL_CMDTMP.arg(schemaName, tn); }


zTable zSQL::getTable(const QString& schemaName, const QString& tablanev, const QStringList& fl){
    zTable t;
    QString cmd;

    if(db.isValid() && db.isOpen()){
        if(driverName == QODBC)
        {
            cmd = getTable_MSSQL_CMD(schemaName, tablanev);
        }
        else if(driverName == QMYSQL)
        {
            cmd = getTable_MYSQL_CMD(schemaName, tablanev);
        }        
        else
        {
            zlog.error("getTable: unknown driver:" + driverName);
        }
    }
    else
    {
        zlog.error("getTable: db closed" + driverName);
    }

    if(!cmd.isEmpty()){
        t= getTable_SQL(tablanev, cmd, fl);

        QString classNamePlural;
        auto className = zTable::getClassName(t.sql_table, classNamePlural);
        t.initClass(className, classNamePlural);
    }

    return t;
}

zTable zSQL::getTable_SQL(const QString& tablanev, const QString& cmd, const QStringList& fl)
{
    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);

    QList<zTablerow> tr;

    while (query.next())
    {
        QString colName = query.value(QStringLiteral("COLUMN_NAME")).toString();

        if(fl.isEmpty() || fl.contains(colName))
        {
            QString dtype = query.value(QStringLiteral("DATA_TYPE")).toString();
            int dlen = query.value(QStringLiteral("CHARACTER_MAXIMUM_LENGTH")).toInt();

            bool nullable = zStringHelper::toBool(query.value(QStringLiteral("IS_NULLABLE")).toString());

            QString caption = zTable::getCaption(colName);//zConversionMap::external(globalCaptionMaps, colName);
            //if(caption.isEmpty()) caption = colName;
            tr.append(zTablerow(colName, dtype, dlen, nullable, caption));
        }
    }

    QString pkn = getTablePKName(tablanev);

    //QList<zTablerow> pr;

// a táblanév táblanév legyen - az sqlből kell a szerver account, a séma név és a tábla név - ezek az sql forráshoz kötődnek
// QString sql_conn;
// QString sql_schema;
// QString sql_table;//sql_table;
    auto e = zTable(tablanev, (pkn.isEmpty()?QStringLiteral("zId"):pkn), tr);//, SQL, tablanev, zStringHelper::Empty);

    return e;
}


/*
PKname
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

QString zSQL::getTable_MSSQL_PK(const QString& tn){
    return getTable_MSSQL_PKTMP.arg(tn);//.arg(this->databaseName);
}

QString zSQL::getTable_MYSQL_PK(const QString& tn){
    return getTable_MYSQL_PKTMP.arg(tn);//.arg(this->databaseName);
}

QString zSQL::getTable_SQL_PK(const QString& cmd)
{
    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);

    if(query.next()) {
        return query.value(QStringLiteral("COLUMN_NAME")).toString();
        }

    return zStringHelper::Empty;
}


QString zSQL::getTablePKName(const QString& tablanev){
    if(db.isValid() && db.isOpen())
    {
        if(driverName == QODBC)
        {
            return getTable_SQL_PK(getTable_MSSQL_PK(tablanev));
        }
        if(driverName == QMYSQL)
        {
            return getTable_SQL_PK(getTable_MYSQL_PK(tablanev));
        }
        zlog.error("getTable: unknown driver:" + driverName);
    }
    else
    {
        zlog.error("getTable: db closed" + driverName);    
    }
    return zStringHelper::Empty;
}



const QString zSQL::getTable_SQL_ENUMTMP = QStringLiteral("SELECT id, %1 as name FROM %2");

QMap<int, QString> zSQL::getTable_SQL_ENUM(const QString& tablanev, const QString& mezonev)
{
    QString cmd = getTable_SQL_ENUMTMP.arg(mezonev, tablanev);

    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);
    auto e = QMap<int, QString>();

    while(query.next())
    {
        auto i = query.value(QStringLiteral("id")).toInt();
        auto n = query.value(QStringLiteral("name")).toString();
        e.insert(i, n);
    }

    return e;
}

/*
Adatbázi funkciók - Séma kezelésével kapcsolatban
*/


QStringList zSQL::getSchemaNames(){
    if(db.isValid() && db.isOpen()){
        if(driverName == QODBC)
        {
            return getSchemaNames_SQL(getSchemaNames_MSSQL_CMD());//.arg(beallitasok.adatbazisNev);
        }
        if(driverName == QMYSQL)
        {
            return getSchemaNames_SQL(getSchemaNames_MYSQL_CMD());
        }
        zlog.error("getDbNames: unknown driver:" + driverName);
    }
    else{
        zlog.error("getDb: db closed" + driverName);
    }
    return QStringList();
}

QStringList zSQL::getSchemaNames_SQL(const QString& cmd) {
    QStringList eredmeny;

    QSqlQuery query(cmd, db);
    query.setForwardOnly(true);

    while (query.next())
    {
        eredmeny.append(query.value(QStringLiteral("schema_name")).toString());
    }
    return eredmeny;
}

QString zSQL::getSchemaNames_MSSQL_CMD(){ return getSchemaNames_MSSQL_CMDTMP; }
QString zSQL::getSchemaNames_MYSQL_CMD(){ return getSchemaNames_MYSQL_CMDTMP; }

/**
  Adatbázisok nevét adó lekérdezések - select datatabase names from mysql server

  https://stackoverflow.com/questions/147659/get-list-of-databases-from-sql-server
  https://www.cyberciti.biz/faq/mysql-command-to-show-list-of-databases-on-server/
 */

const QString zSQL::getSchemaNames_MYSQL_CMDTMP = QStringLiteral("SELECT DISTINCT TABLE_SCHEMA as schema_name FROM INFORMATION_SCHEMA.TABLES "
                                              "WHERE TABLE_SCHEMA not in('mysql', 'performance_schema', 'information_schema','sys') ORDER BY schema_name" );

const QString zSQL::getSchemaNames_MSSQL_CMDTMP = QStringLiteral("SELECT name AS schema_name FROM sys.databases WHERE name NOT IN ('master', 'tempdb', 'model', 'msdb') ORDER BY schema_name");
