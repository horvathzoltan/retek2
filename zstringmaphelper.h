#ifndef ZSTRINGMAPHELPER_H
#define ZSTRINGMAPHELPER_H

#include <QString>
#include <QMap>

class zStringMapHelper
{
public:
    zStringMapHelper();
    static void StringMapFeltolt(QString fn, QMap<QString, QString> *map);
    static void StringMapSave(QString, QMap<QString, QString> *map);
};

#endif // ZSTRINGMAPHELPER_H
