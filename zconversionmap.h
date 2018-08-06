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
    QString value(const QString&) const;
    QStringList keys(const QString& value);
public:    
    zConversionMap();
    static QList<zConversionMap> loadAll(const QString& filePath, const QStringList &);
    static zConversionMap load(const QString& fileName);
    static QString value(const QList<zConversionMap>&, const QString&);

    //static QStringList keys(const QList<zConversionMap>& maps);
    static QStringList keys(const QList<zConversionMap>& maps,const QString& value = zStringHelper::Empty);

};

#endif // ZCAPTIONMAP_H
