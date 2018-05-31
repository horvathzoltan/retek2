#ifndef ZTABLE_H
#define ZTABLE_H

#include <QList>
#include <QString>
#include <QDateTime>

#include "ztablerow.h"

enum zTableSourceTypes:int { SQL=0, TXT=1 };

class zTable
{
public:
    zTable(QString tablanev, QString pkn, QList<zTablerow>, QList<zTablerow>, int type);    

    zTable();
    ~zTable();

    int sourcetype;
    QString sourcepath;

    QString tablename;
    QString classname; // osztálynév - singular, ezt a zTables példány létrehozásakor létre lehet már hozni, és validálni egy szabály szerint
    QString classname_plural;
    QString comment;

    QString pkname; // rowix

    QList<zTablerow> rows;
    QList<zTablerow> props;
    QString name_formatstring;

    QDateTime updateTime;


    //static zTable LoadFromSQL(QString, QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMSSQL(QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMySQL(QString, QMap<QString, QString>, QString );

    QString toString();

    QList<QString> Validate(zTable);

    static QList<zTable> createTableByText(QString txt);    
    static QList<zTable> createTableByText_2(QString txt);
    static QList<zTable> createTableByXML(const QString& txt);

    static QString getPkByName(QList<zTable> *tables, QString rn);
    static zTable* getByName(QList<zTable> *tables, QString rn);
    //void getType(auto re_dlen1, QString dtype, auto re_dlen2, QString ezt1, bool isDtype, int dlen);

    static bool getType(QString ezt1,  QString *dtype, int *dlen);

    QStringList getFK();
    QStringList getRFK();

    bool containsRow(QString n);

    void toXML(QXmlStreamWriter*);
    //static zTable fromXML(QString);
    static zTable fromXML(QXmlStreamReader* xml);
};

#endif // ZTABLE_H
