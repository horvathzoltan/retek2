#ifndef ZCAPTIONMAP_H
#define ZCAPTIONMAP_H

#include <QMap>
#include <QString>



class zCaptionMap
{
private:
    QString name;
    QMap<QString, QString> map;
    static const QString Empty;
public:    
    zCaptionMap();
    static QList<zCaptionMap> loadAll(const QString& filePath);
    static zCaptionMap load(const QString& fileName);
    static QString value(const QList<zCaptionMap>&, const QString&);
    QString value2(const QString&) const;
};

#endif // ZCAPTIONMAP_H
