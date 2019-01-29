#ifndef ZSTRINGMAPHELPER_H
#define ZSTRINGMAPHELPER_H

#include <QString>
#include <QMap>

class zStringMapHelper
{
public:
    zStringMapHelper();
    static void StringMapFeltolt(const QString& fn, QMap<QString, QString> *map);
    static void StringMapSave(const QString&, QMap<QString, QString> *map);
    static bool contains(QMap<QString, QString> *map, const QString& k);
    static QString getKey(QMap<QString, QString> *map, const QString& k);
};

#endif // ZSTRINGMAPHELPER_H
