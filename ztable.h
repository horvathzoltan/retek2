#ifndef ZTABLE_H
#define ZTABLE_H

#include <QList>
#include <QString>
#include <QDateTime>
#include "zconversionmap.h"
//#include "zstringhelper.h"
#include "ztableerror.h"
#include "ztablerow.h"

//enum zTableSourceTypes:int { SQL=0, TXT=1, ENTITY=2 };

enum zTableSearchBy{ Name, TableName, class_name, class_namePlural };

class zTable
{
public:        
    static const QString VALIDATETABLEKEY;
    static const QString VALIDATESQLKEY;
    static const QString VALIDATEDOCKEY;
    static const QString VALIDATESRCKEY;

    //zTable(QString class_name, const QString& , const QList<zTablerow>&, int type, QString name, QString _sourcepath);
    zTable(const QString& name, const QString& pkName , const QList<zTablerow>&);

    void initSql(const QString&, const QString&, const QString&);
    void initClass(const QString& className,const QString &pluralClassName = zStringHelper::Empty);
    void initClassByName();
    static QString getClassName(const QString& n, QString&);

    zTable();
    ~zTable();

    QString name;     // megnevezés - a tábla egyedi azonosítója

    // sql conn: conn_név + driver + server + user + pass (pl: deathstar)
    // sql adatbázis_név (pl: gloster)
    // sql tábla név (pl: atricles)
    QString sql_conn;// ez a conn neve, de lehetne a conn is - pointerrel
    QString sql_schema;
    QString sql_table;//sql_table;
    bool sql_isValid;
    QDateTime sql_updateTimeStamp;

    QString class_path;//class_path // ez pedig az az entitás, ami a forráskódban osztályként írja le az adatot - elvileg ez egy adott osztályt tartalmazó file teljes neve
    QString class_name; // osztálynév - singular, ezt a zTables példány létrehozásakor létre lehet már hozni, és validálni egy szabály szerint
    QString class_name_plural; // kollekció - osztálynév
    bool source_isValid;
    QDateTime source_updateTimeStamp;

    QString docName;
    QString document_path;
    bool document_isValid;
    QDateTime document_updateTimeStamp;

    QString XMLPath;
    QString comment;
    //QString pkname; // rowix
    int pkrowix = -1;
    QList<zTablerow> rows;

    // a megnevezés képzésének szabálya, leírója
    QString name_formatstring;
    QDateTime updateTime;

    QList<zTableError> eval;

    //static zTable LoadFromSQL(QString, QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMSSQL(QString, QMap<QString, QString>, QString );
    //static zTable LoadFromMySQL(QString, QMap<QString, QString>, QString );

    QString toString() const;

    bool Compare(const zTable &, QList<zTableError>& e, const QString& source);

    static QList<zTable> createTableByText(QString txt);    
    static QList<zTable> createTableByText_2(QString txt);
    static QList<zTable> createTableByXML(const QString& txt);
    static QList<zTable> createTableByHtml(const QString& txt, const QString &d=zStringHelper::Empty);

    static QList<zTable> createTableByText_3(const QString& txt, QMap<QString, QString>*, QMap<QString, QString>* = nullptr);

    static QString getPkByTableName(QList<zTable> *tables, const QString& rn);
    static const zTable* find(const QList<zTable>& tables, const QString& rn, zTableSearchBy searchType = zTableSearchBy::TableName);
    //void getType(auto re_dlen1, QString dtype, auto re_dlen2, QString ezt1, bool isDtype, int dlen);

    //static bool getClassType_old(const QString& ezt1,  QString *dtype, int *dlen, bool *nullable, bool isRequired);

    // TODO milyen irányból - sql, class - állapítjuk meg a típust?
    static bool getClassType(const QList<zConversionMap>& maps, const QString& ezt1,  QString *dtype, int *dlen, bool *nullable, bool isRequired, bool isWarning = true);

    QStringList getFK();
    QStringList getRFK();

    bool containsRow(const QString& n);

    void toXML(QXmlStreamWriter*);
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

    bool Validate(const QList<zTable>& tables, QList<zTableError> &e, const QString &source, QStringList &err);
    static QString getCaption(const QString& fname);

    bool validateSQL();
    bool validateSource();
    bool validateDocument();

    QDateTime getSqlUpdateTimestamp() const;
    QDateTime getSourceUpdateTimestamp();
    QDateTime getDocUpdateTimestamp();
    
    QString pkname() const;
    bool hasPkname() const;
    QString SqlConnToString() const;
    static QString createTxtByHtml(const QString &txt);
    QString getName() const;
private:
    static const QString PKNAME;
    static const QRegularExpression re_dlen3;
    //void validateEval(bool isOK, const QStringList& e, const QString&);

//    enum class ErrCode:int;
    enum class ErrCode:int{noteq, unknown, notexists, notunique};
    static const QMap<ErrCode, QString> ErrCodeNames;
    static const QMap<ErrCode, QString> ErrCodeDescriptions;

    //QString GetErrorMessage(const QString& cn, ErrCode code);
    zTableError GetFullError(const QString& cn, ErrCode code, const QStringList &p, const QString &prefix) const;


};

#endif // ZTABLE_H

