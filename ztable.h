#ifndef ZTABLE_H
#define ZTABLE_H

#include <QString>
#include <QList>

#include "ztablerow.h"

class zTable
{
public:
    zTable(QString tablanev, QString pkn, QList<zTablerow>, QList<zTablerow>);

    zTable();
    ~zTable();

    QString tablename;
    QString pkname;

    QStringList fknames;

    QList<zTablerow> rows;
    QList<zTablerow> props;

    QString name_formatstring;

    //static zTable LoadFromSQL(QString, QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMSSQL(QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMySQL(QString, QMap<QString, QString>, QString );

    QString toString();

    QList<QString> Validate(zTable);

    static QList<zTable> createTableByText(QString txt);    
    static QList<zTable> createTableByText_2(QString txt);

    static QString getPkByName(QList<zTable> *tables, QString rn);
    static zTable* getByName(QList<zTable> *tables, QString rn);
    //void getType(auto re_dlen1, QString dtype, auto re_dlen2, QString ezt1, bool isDtype, int dlen);

    static bool getType(QString ezt1,  QString *dtype, int *dlen);
};

#endif // ZTABLE_H
