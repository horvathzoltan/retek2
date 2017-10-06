#include <QSqlQuery>
#include "ztable.h"

zTable zTable::LoadFromSQL(QString tablanev)
{
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

    /*
    COLUMN_NAME	DATA_TYPE	CHARACTER_MAXIMUM_LENGTH	NUMERIC_PRECISION	NUMERIC_SCALE	IS_NULLABLE	IsPartOfPrimaryKey
    id	int	NULL	10	0	NO	1
    idopont	datetime	NULL	NULL	NULL	YES	0
    */
    int r_ix = 0;
    /*uint c_ix_colName = 0;
    uint c_ix_dtype = 1;
    uint c_ix_dlen = 2;*/
    /*
    entitásfile
    */
    while (query.next()) {
        QString colName = query.value("COLUMN_NAME").toString();
        QString dtype = query.value("DATA_TYPE").toString();
        QString dlen = query.value("CHARACTER_MAXIMUM_LENGTH").toString();
        QString nullable = query.value("IS_NULLABLE").toString();

        ui.tableWidget_MezoLista->insertRow(r_ix);

        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_colName, new QTableWidgetItem(colName));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_colType, new QTableWidgetItem(dtype));
        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_dlen, new QTableWidgetItem(dlen));

        QString caption;
        QString cn = colName.toLower();

        if (tablaCaptionMap.contains(cn))
            caption = tablaCaptionMap[cn];
        else if (globalCaptionMap.contains(cn))
            caption = globalCaptionMap[cn];
        /*else
            caption = "?";*/
        //colName, dtype, dlen, caption, nullable

        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_Caption, new QTableWidgetItem(caption));

        ui.tableWidget_MezoLista->setItem(r_ix, C_ix_nullable, new QTableWidgetItem(nullable));
        r_ix++;
    }
}
