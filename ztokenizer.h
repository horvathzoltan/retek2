#ifndef ZTOKENIZER_H
#define ZTOKENIZER_H

#include <QString>
#include <QTableWidget>
#include <QVariant>


class zTokenizer
{
private:
    QTableWidget* MezoLista;
    QMap<QString, QString> dxMap;
    //QMap<QString, QString> tmpMap;
    void feltoltTmpMap();
public:
    zTokenizer();
    void init(QTableWidget*);

    static const QString TXT;


    void tokenize(QString *tmp, QMap<QString, QVariant> *, int, const QString &dbname);
    int tokenizeR(QString *txt, int ix, int*, QMap<QString, QVariant>*, int, const QString& dbname);
    QString getToken(const QString& t1, const QString& t2, QMap<QString, QVariant> *map, int, QString dbname);

    //QString getAdatbazisnev(void);
    QString getCaptionFileName(QString);
    //QString getOsztalynevLower(QString tablanev);

    QString getContextNev();

    //QString get_liNev(QString o);

    //QString getPropList();
    QString getPropList2(const QString& tmp, const QString& param, int whsp, const QString& dbname);

    QString getAttrList(QMap<QString, QVariant> *map, int whsp);

    QString getPropType(const QString& tipusnev, bool isnullable);
    QString getePropType(const QString& tipusnev, int length);
    //QString getReq(QString tmp, QMap<QString, QVariant> *map);

    QString getEntityAttrList(QMap<QString, QVariant> *map, int whsp);
    QString getEntityPropAttrList(QMap<QString, QVariant> *map, int whsp);
    QString getEntityNavPropList(const QString& tmp, const QString& param, int whsp, const QString& dbname);
    //static void AttrListAdd(QStringList& e, const QString& string, int whsp);
    static QString AttrListJoin(const QStringList& e, int w);
    QString getProp(const QString &propType, const QString& propName, const QString& tmp, int whsp, const QString& dbname);
};

#endif // ZTOKENIZER_H
