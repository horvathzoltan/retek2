#ifndef ZSTRINGMAPHELPER_H
#define ZSTRINGMAPHELPER_H

#include <QString>
#include <QMap>

class zstringmaphelper
{
public:
    zstringmaphelper();
    static void StringMapFeltolt(QString fn, QMap<QString, QString> *map);
    static void StringMapSave(QString, QMap<QString, QString> *map);
};

#endif // ZSTRINGMAPHELPER_H
