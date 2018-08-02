#ifndef ZTABLE_H
#define ZTABLE_H

#include <QList>
#include <QString>
#include <QDateTime>

#include "ztablerow.h"

enum zTableSourceTypes:int { SQL=0, TXT=1, ENTITY=2 };

enum zTableSearchBy{ Name, TableName, class_name, class_namePlural };

class zTable
{
public:
    zTable(QString class_name, const QString& , const QList<zTablerow>&, int type, QString name, QString _sourcepath);

    zTable();
    ~zTable();

    QString name;

    // kettő féle conn van - egy az sql felé, egy a forrás felé - ha ezek teljesülnek, a tábla teljesen be van kötve
    // sql conn: conn_név + driver + server + user + pass (pl: deathstar)
    // sql adatbázis_név (pl: gloster)
    // sql tábla név (pl: atricles)
    QString sql_conn;
    QString sql_schema;
    QString sql_table;//sql_table;

    QString class_path;//class_path // ez pedig az az entitás, ami a forráskódban osztályként írja le az adatot - elvileg ez egy adott osztályt tartalmazó file teljes neve
    QString class_name; // osztálynév - singular, ezt a zTables példány létrehozásakor létre lehet már hozni, és validálni egy szabály szerint
    QString class_name_plural; // killekció - osztálynév

    QString comment;
    QString pkname; // rowix
    QList<zTablerow> rows;

    // a megnevezés képzésének szabálya, leírója
    QString name_formatstring;
    QDateTime updateTime;

    //static zTable LoadFromSQL(QString, QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMSSQL(QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMySQL(QString, QMap<QString, QString>, QString );

    QString toString();

    QList<QString> Compare(zTable);

    static QList<zTable> createTableByText(QString txt);    
    static QList<zTable> createTableByText_2(QString txt);
    static QList<zTable> createTableByXML(const QString& txt);

    static QList<zTable> createTableByText_3(const QString& txt, QMap<QString, QString>*, QMap<QString, QString>* = nullptr);

    static QString getPkByTableName(QList<zTable> *tables, const QString& rn);
    static zTable* find(QList<zTable> *tables, const QString& rn, zTableSearchBy searchType = zTableSearchBy::TableName);
    //void getType(auto re_dlen1, QString dtype, auto re_dlen2, QString ezt1, bool isDtype, int dlen);

    static bool getType(const QString& ezt1,  QString *dtype, int *dlen, bool *nullable, bool isRequired);

    QStringList getFK();
    QStringList getRFK();

    bool containsRow(const QString& n);

    void toXML(QXmlStreamWriter*);
    //static zTable fromXML(QString);
    static zTable fromXML(QXmlStreamReader* xml);
    QStringList getFKclass_name();
    QStringList getRFKclass_namePlural();

    static QStringList getAttrAndParams(const QString& str);
    static QString getFirstNotNull(const QRegularExpressionMatch& m,  int);
    static QString getConstFromArgument(const QString& str);

    static QList<zTable> createTableByClassTxt(const QString& txt);

//    static QRegularExpression r_class;
//    static QRegularExpression r_attr;
//    static QRegularExpression r_class_or_attr;
//    static QString p_class;
    //    static QString p_attr;
    void saveTablaToXML();

    bool Validate(bool = false);
};

#endif // ZTABLE_H

