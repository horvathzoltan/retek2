#ifndef ZCAPTIONMAP_H
#define ZCAPTIONMAP_H

#include <QMap>
#include <QString>


/*
Alfanumerikus kulcs és érték közti konverziót írnak le
*/
class zConversionMap
{
private:
    QString name;
    QMap<QString, QString> map;
    static const QString Empty;
public:    
    zConversionMap();
    static QList<zConversionMap> loadAll(const QString& filePath, const QStringList &);
    static zConversionMap load(const QString& fileName);
    static QString value(const QList<zConversionMap>&, const QString&);
    QString value2(const QString&) const;
};

#endif // ZCAPTIONMAP_H
