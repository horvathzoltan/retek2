#ifndef ZCAPTIONMAP_H
#define ZCAPTIONMAP_H

#include <QDateTime>
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
    QString fullname;
    QString name;
    QDateTime lastloaded;

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

    static void reLoadAll(QList<zConversionMap>& maps, const QString& filePath, const QStringList &);
    //static zConversionMap reLoad(const QString& fileName);
    static int indexByFullName(const QList<zConversionMap>& maps, const QString& fullName);
};

#endif // ZCAPTIONMAP_H
