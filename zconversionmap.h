#ifndef ZCAPTIONMAP_H
#define ZCAPTIONMAP_H

#include <QMap>
#include <QString>
#include "zstringhelper.h"
#include "zconversionstruct.h"

/*
Alfanumerikus kulcs és érték közti konverziót írnak le
*/
class zConversionMap
{
private:
    QString name;
    //QMap<QString, QString> map;
    QList<zConversionStruct> list;
    //static const QString Empty;
    QString external(const QString&) const;
    QStringList internals(const QString& external =zStringHelper::Empty ) const;
public:    
    zConversionMap();
    static QList<zConversionMap> loadAll(const QString& filePath, const QStringList &);
    static zConversionMap load(const QString& fileName);
    static QString external(const QList<zConversionMap>&, const QString&);

    static QStringList internals(const QList<zConversionMap>& maps,const QString& external = zStringHelper::Empty);

    static void load(const QString& fn, QList<zConversionStruct> *list);
};

#endif // ZCAPTIONMAP_H
