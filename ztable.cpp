#include <QSqlQuery>
//#include <QVector>
//#include <QMap>
#include <QVariant>

#include "globals.h"
#include "ztablerow.h"
#include "ztable.h"
#include "zstringmaphelper.h"

zTable::zTable(){};

zTable::~zTable(){};

zTable::zTable(QString n, QList<zTablerow> tr){
    this->tablename = n;
    this->rows = tr;
}


//zTable zTable::LoadFromSQL(QString tablanev, QMap<QString, QString> globalCaptionMap, QString fn)
//{

//    QString commandTextTemplate = "Select "
//        "C.COLUMN_NAME, "
//        "C.DATA_TYPE, "
//        "C.CHARACTER_MAXIMUM_LENGTH, "
//        "C.NUMERIC_PRECISION, "
//        "C.NUMERIC_SCALE, "
//        "C.IS_NULLABLE, "
//        "Case When Z.CONSTRAINT_NAME Is Null Then 0 Else 1 End As IsPartOfPrimaryKey From INFORMATION_SCHEMA.COLUMNS "
//        "As C Outer Apply("
//        "Select CCU.CONSTRAINT_NAME From INFORMATION_SCHEMA.TABLE_CONSTRAINTS As TC "
//        "Join INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE As CCU "
//        "On CCU.CONSTRAINT_NAME = TC.CONSTRAINT_NAME "
//        "Where TC.TABLE_SCHEMA = C.TABLE_SCHEMA And TC.TABLE_NAME = C.TABLE_NAME And TC.CONSTRAINT_TYPE = 'PRIMARY KEY' And CCU.COLUMN_NAME = C.COLUMN_NAME) As Z "
//        "Where C.TABLE_NAME = '%1'";


//    QString commandText = commandTextTemplate.arg(tablanev);



//    QSqlQuery query(commandText);

//    QList<zTablerow> tr;

//    if(query.next()){
//        QMap<QString, QString> tablaCaptionMap;
//        zStringMapHelper::StringMapFeltolt(fn, &tablaCaptionMap);

//        while (query.next()) {
//            QString colName = query.value("COLUMN_NAME").toString();
//            QString dtype = query.value("DATA_TYPE").toString();
//            int dlen = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();
//            bool nullable = query.value("IS_NULLABLE").toBool();
//            QString cn = colName.toLower();

//            QString caption = tablaCaptionMap.contains(cn)?tablaCaptionMap[cn]:globalCaptionMap.contains(cn)?globalCaptionMap[cn]:cn;

//            auto r = zTablerow(colName, dtype, dlen, nullable, caption);

//            tr.append(r);
//        }

//        auto e = zTable(tablanev, tr);
//        return e;
//    }
//    else{
//        //QList<zTablerow> trl;
//        return zTable("", QList<zTablerow>());
//    }
//}

//zTable zTable::LoadFromMySQL(QString tablanev, QMap<QString, QString> globalCaptionMap, QString fn)
//{
//    QString commandTextTemplate = "SELECT "
//                "COLUMN_NAME,"
//                "DATA_TYPE,"
//                "CHARACTER_MAXIMUM_LENGTH,"
//                "NUMERIC_PRECISION,"
//                "NUMERIC_SCALE,"
//                "IS_NULLABLE"
//            "FROM INFORMATION_SCHEMA.COLUMNS "
//            "where table_schema = '%1' and table_name = '%2'";

//    QString commandText = commandTextTemplate.arg("wiki1").arg(tablanev);



//    QSqlQuery query(commandText);

//    QList<zTablerow> tr;

//    if(query.next()){
//        QMap<QString, QString> tablaCaptionMap;
//        zStringMapHelper::StringMapFeltolt(fn, &tablaCaptionMap);

//        while (query.next()) {
//            QString colName = query.value("COLUMN_NAME").toString();
//            QString dtype = query.value("DATA_TYPE").toString();
//            int dlen = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();
//            bool nullable = query.value("IS_NULLABLE").toBool();
//            QString cn = colName.toLower();

//            QString caption = tablaCaptionMap.contains(cn)?tablaCaptionMap[cn]:globalCaptionMap.contains(cn)?globalCaptionMap[cn]:cn;

//            auto r = zTablerow(colName, dtype, dlen, nullable, caption);

//            tr.append(r);
//        }

//        auto e = zTable(tablanev, tr);
//        return e;
//    }
//    else{
//        //QList<zTablerow> trl;
//        return zTable("", QList<zTablerow>());
//    }
//}

QString zTable::toString(){
    QString rs;
    zforeach(r, this->rows){
        if(!rs.isEmpty()) rs+=",";
        rs+=r->toString();
    }
    return  this->tablename+"("+rs+")";
}


QList<QString> zTable::Validate(zTable t){
    QList<QString> e;

    if(this->tablename!=t.tablename)
        e.append("Tablename not equals");
    else
        e.append("Tablename OK");

    zforeach(r,this->rows){
        int i = t.rows.indexOf(*r);

        e.append(QString((*r).colName)+QString::number(i));
    }
    return e;
}
