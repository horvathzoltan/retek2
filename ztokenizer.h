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
    QMap<QString, QString> tmpMap;
    void feltoltTmpMap(void);
public:
    zTokenizer();
    void init(QTableWidget*);

    static const QString TXT;


    void tokenize(QString *tmp, QMap<QString, QVariant> *, int, QString dbname);
    int tokenizeR(QString *txt, int ix, int*, QMap<QString, QVariant>*, int, QString dbname);
    QString getToken(QString t1, QString t2, QMap<QString, QVariant> *map, int, QString dbname);

    //QString getAdatbazisnev(void);
    QString getCaptionFileName(QString);
    QString getOsztalynevLower(QString tablanev);

    QString getContextNev(void);

    QString get_liNev(QString o);

    QString getPropList();
    QString getPropList2(QString tmp, QString param, int whsp, QString dbname);

    QString getAttrList(QMap<QString, QVariant> *map, int whsp);

    QString getPropType(QString tipusnev, bool isnullable);
    QString getePropType(QString tipusnev, int length);
    //QString getReq(QString tmp, QMap<QString, QVariant> *map);

};

#endif // ZTOKENIZER_H
