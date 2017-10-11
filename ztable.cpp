#include <QSqlQuery>
//#include <QVector>
//#include <QMap>
#include <QVariant>

#include "ztablerow.h"
#include "ztable.h"
#include "zstringmaphelper.h"

zTable::zTable(QString n){
    this->tablename = n;
    this->rows = QVector<zTablerow>();
}



zTable zTable::LoadFromSQL(QString tablanev, QMap<QString, QString> globalCaptionMap, QString fn)
{
    QMap<QString, QString> tablaCaptionMap;
    zstringmaphelper::StringMapFeltolt(fn, &tablaCaptionMap);

    QString commandTextTemplate = "Select "
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

    QString commandText = commandTextTemplate.arg(tablanev);

    QSqlQuery query(commandText);   

    auto e = zTable(tablanev);

    while (query.next()) {
        QString colName = query.value("COLUMN_NAME").toString();
        QString dtype = query.value("DATA_TYPE").toString();
        int dlen = query.value("CHARACTER_MAXIMUM_LENGTH").toInt();
        bool nullable = query.value("IS_NULLABLE").toBool();
        QString cn = colName.toLower();

        QString caption = tablaCaptionMap.contains(cn)?tablaCaptionMap[cn]:globalCaptionMap.contains(cn)?globalCaptionMap[cn]:cn;

        auto r = zTablerow(colName, dtype, dlen, nullable, caption);

        e.rows.append(r);
    }

    return e;
}
