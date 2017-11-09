#ifndef ZTOKENIZER_H
#define ZTOKENIZER_H

#include <QString>
#include <QVariant>
#include <QTableWidget>

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


    void tokenize(QString *tmp, QMap<QString, QVariant> *, int);
    int tokenizeR(QString *txt, int ix, int*, QMap<QString, QVariant>*, int);
    QString getToken(QString t1, QString t2, QMap<QString, QVariant> *map, int);

    //QString getAdatbazisnev(void);
    QString getCaptionFileName(QString);
    QString getOsztalynevLower(QString tablanev);
    QString getOsztalynevUpper(QString tablanev);
    QString getContextNev(void);

    QString get_liNev(QString o);

    QString getPropList();
    QString getPropList2(QString tmp, QString param, int whsp);

    QString getAttrList(QString tmp, QMap<QString, QVariant> *map, int whsp);

    QString getPropType(QString tipusnev, int length, bool isnullable);
    QString getePropType(QString tipusnev, int length, bool isnullable);
    //QString getReq(QString tmp, QMap<QString, QVariant> *map);

};

#endif // ZTOKENIZER_H
